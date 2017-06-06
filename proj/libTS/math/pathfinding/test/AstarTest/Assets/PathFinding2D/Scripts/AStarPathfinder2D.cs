using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

namespace Tsl.Math.Pathfinder
{
    public class AStarPathfinder2D : AStarPathfinder2Dbase
    {
        public bool GridMode = true; // 最適化時に、結果をグリッドのリストに変換する

        public static AStarPathfinder2D Instance;

        void Awake()
        {
            AStarPathfinder2D.Instance = this;
        }

        public override void MakeRelation(AstarCell cell)
        {
            setGridRelatedSearchRaycast(cell, true);
        }


        protected override void Goal(AstarCell cell)
        {
            base.Goal(cell);
            if (this.GridMode)
            {
                this.pathList = fillGrid(this.pathList);
            }
        }

        private List<Vector2> fillGrid(List<Vector2> lines)
        {
            List<Vector2> result = new List<Vector2>();
            for (int i = 0; i < lines.Count - 1; ++i)
            {
                if (i == 0) result.Add(lines[i]);
                AStarPathfinder2D.Instance.RaycastCell(lines[i], lines[i + 1], (x, y) =>
                  {
                      var cell = AStarPathfinder2D.Instance.CellMap(x, y);
                      if (cell.CellType == AstarCell.Type.Removed) cell.CellType = AstarCell.Type.SkipPoint;
                      result.Add(new Vector2(x, y));
                      return false;
                  });
            }
            return result;
        }

        // 到達可能なノードを全ノードからレイキャストして調べる
        public void setGridRelatedSearchRaycast(AstarCell parent, bool newCell = false)
        {
            parent.ClearRelated();
            foreach (var cell in this.cellMapBody.Where(c => c.IsValidCell()))
            {
                if (cell == parent) continue;
                var fromCell = cell.Find(parent);
                if (fromCell.cell != null)
                {   // 相手から自分が見えている場合
                    if (!parent.Contains(cell))
                    {
                        parent.AddRelated(cell, fromCell.cost);
                    }
                    continue;
                }
                // raycast
                float cost = 0;
                var prevPos = parent.Position;
                RaycastCell(parent.Position, cell.Position,
                        (cx, cy) =>
                        {
                            if (this.GridMode)
                            {
                                var nowpos = new Vector2(cx, cy);
                                cost += (nowpos - prevPos).magnitude;
                                prevPos = nowpos;
                            }
                            var type = this.CellType(cx, cy);
                            if (type != AstarCell.Type.Removed)
                            {   // 何かあった
                                if (type != AstarCell.Type.Block)
                                {   // ブロックもしくは圏外ではない場合
                                    var rcell = this.CellMap(cx, cy);
                                    if (rcell == cell)
                                    {   // 見つかった!
                                        if (!parent.Contains(cell))
                                        {
                                            if (!this.GridMode) cost = (cell.Position - parent.Position).magnitude;
                                            parent.AddRelated(cell, cost);
                                            if (newCell && !cell.Contains(parent))
                                            {
                                                cell.AddRelated(parent, cost);
                                            }
                                        }
                                    }
                                }
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        });
            }
        }

        // DDAでレイキャスト
        // (src,target] までDDAでトレースする
        public void RaycastCell(Vector2 src, Vector2 target, System.Func<float, float, bool> act)
        {
            float x = src.x;
            float y = src.y;
            float sx = x > target.x ? -this.TileSize : this.TileSize;
            float sy = y > target.y ? -this.TileSize : this.TileSize;
            float dx = Mathf.Abs(target.x - x);
            float dy = Mathf.Abs(target.y - y);
            if (dx == dy)
            {   //ななめ45度
                while (true)
                {
                    x += sx;
                    y += sy;
                    dx -= this.TileSize;
                    if (dx < 0.0f) break;
                    if (act(x, y)) return;
                }
            }
            else if (dx > dy)
            {   // 横に長い
                float r = dx * 0.5f;
                while (sx > 0 ? x < target.x : x > target.x)
                {
                    x += sx;
                    r -= dy;
                    if (r < 0.0f)
                    {
                        r += dx;
                        y += sy;
                    }
                    if (act(x, y)) return;
                }
            }
            else
            {   // 縦に長い
                float r = dy * 0.5f;
                while (sy > 0 ? y < target.y : y > target.y)
                {
                    y += sy;
                    r -= dx;
                    if (r < 0.0f)
                    {
                        r += dy;
                        x += sx;
                    }
                    if (act(x, y)) return;
                }
            }
        }

        public void MapMake()
        {
            foreach (var cell in this.cellMapBody)
            {
                if (cell.CellType != AstarCell.Type.Block)
                    cell.CellType = AstarCell.Type.Empty;
                cell.Related.Clear();
            }

            // 基本的なAStartでは処理が冗長のため、最適化を行う
            // 8方向全部がEmptyであるセルを削除
            var octcells = this.cellMapBody.Where(c =>
            {
                if (c.CellType != AstarCell.Type.Empty) return false;
                for (int ix = -1; ix < 2; ++ix)
                {
                    for (int iy = -1; iy < 2; ++iy)
                    {
                        if (ix == 0 && iy == 0) continue;
                        if (this.CellType(c.Position.x + (ix * this.TileSize), c.Position.y + (iy * this.TileSize))
                        != AstarCell.Type.Empty) return false;
                    }
                }
                return true;
            }
            ).ToList();
            foreach (var cell in octcells)
            {
                cell.CellType = AstarCell.Type.Removed;
            }

            // 角以外のセルを削除
            List<AstarCell> removeList = new List<AstarCell>();
            foreach (var cell in this.cellMapBody.Where(c => c.CellType == AstarCell.Type.Empty))
            {
                var pos = cell.Position;
                AstarCell.Type[,] ar = new AstarCell.Type[3, 3]; // check around
                for (int ix = -1; ix < 2; ++ix)
                {
                    for (int iy = -1; iy < 2; ++iy)
                    {
                        if (ix == 0 && iy == 0) continue;
                        float x = pos.x + (ix * this.TileSize);
                        float y = pos.y + (iy * this.TileSize);
                        ar[ix + 1, iy + 1] = this.CellType(x, y);
                    }
                }
                // 縦横3連ブロックチェック
                for (int inv = 0; inv < 2; ++inv)
                {
                    System.Func<int, int, AstarCell.Type> m = (x, y) =>
                        {
                            return inv == 0 ? ar[x, y] : ar[y, x];
                        };

                    for (int s = 0; s <= 2; s += 2)
                    {
                        int u = 2 - s;
                        if (m(s, 0) == m(s, 1) && m(s, 1) == m(s, 2) && m(s, 0) == AstarCell.Type.Block)
                        {
                            if (m(1, 0) == AstarCell.Type.Empty && m(1, 2) == AstarCell.Type.Empty)
                            {
                                if (m(u, 0) != AstarCell.Type.Block
                                    && m(u, 1) != AstarCell.Type.Block
                                    && m(u, 2) != AstarCell.Type.Block)
                                {
                                    removeList.Add(cell);
                                }
                            }
                            if (m(1, 0) == AstarCell.Type.Empty || m(1, 2) == AstarCell.Type.Empty)
                            {
                                if ((m(1, 2) == AstarCell.Type.Block && m(u, 2) == AstarCell.Type.Block && m(u, 0) != AstarCell.Type.Block)
                                || (m(1, 0) == AstarCell.Type.Block && m(u, 0) == AstarCell.Type.Block && m(u, 2) != AstarCell.Type.Block))
                                {
                                    removeList.Add(cell);
                                }
                            }
                        }
                    }
                }
            }
            foreach (var cell in removeList)
            {
                cell.CellType = AstarCell.Type.Removed;
            }

            foreach (var cell in this.cellMapBody.Where(c => c.CellType == AstarCell.Type.Empty))
            {
                setGridRelatedSearchRaycast(cell);
            }
        }

    }
}

