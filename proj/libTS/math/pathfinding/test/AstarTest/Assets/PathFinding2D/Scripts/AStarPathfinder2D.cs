using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

namespace Tsl.Math.Pathfinder
{
    public class AStarPathfinder2D : MonoBehaviour
    {
        public float TileSize = 1.0f;
        public bool GridMode = true; // 最適化時に、結果をグリッドのリストに変換する

        public static AStarPathfinder2D Instance;

        public enum ExecuteMode
        {
            Sync, // 最後までノンストップ
            ASync, // Coroutineで実行
            Step, // 1ステップずつ
        }

        private AstarCell[] cellMapBody;

        private bool pathfindFinished = false;
        private bool first = true;
        private Rect MapRect = new Rect(0,0,16,16);
        private Vector2 StartPoint = new Vector2(0, 0);
        private Vector2 GoalPoint = new Vector2(15, 15);

        private int pathCount;
        private int GridWidth {  get {  return (int)(this.MapRect.width / this.TileSize);} }
        private int GridHeight {  get {  return (int)(this.MapRect.height / this.TileSize);} }

        private int cellIndex(float x, float y)
        {
            int ix = (int)((x - this.MapRect.x) / this.TileSize + 0.5f);
            int iy = (int)((y - this.MapRect.y) / this.TileSize + 0.5f);
            if ( x < this.MapRect.x || y < this.MapRect.y) return -1;
            if (ix < 0 || ix >= this.GridWidth || iy < 0 || iy >= this.GridHeight) return -1;
            return iy * this.GridWidth + ix;
        }
        public AstarCell CellMap(float x, float y)
        {
            int index = cellIndex(x, y);
            if (index < 0 || index >= cellMapBody.Count())
            {
                Debug.LogError(string.Format("Invalid position: ({0},{1})", x, y));
                return null;
            }
            return cellMapBody[index];
        }

        public AstarCell.Type CellType(float x, float y)
        {
            int index = cellIndex(x, y);
            if (index < 0) return AstarCell.Type.Block;
            return CellMap(x, y).CellType;
        }

        void Awake()
        {
            AStarPathfinder2D.Instance = this;
        }

        // Use this for initialization
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {

        }

        public int NumOfNodes
        {
            get { return this.cellMapBody.Where(c => c.CellType != AstarCell.Type.Removed && c.CellType != AstarCell.Type.Block).Count();}
        }
        public int NumOfLinks
        {
            get { return this.cellMapBody.Where(c => c.CellType != AstarCell.Type.Removed && c.CellType != AstarCell.Type.Block).Select(c => c.Related.Count).Sum(); }
        }

        public int PathCount
        {
            get { return this.pathCount; }
        }

        // intで評価されるmapRectのグリッドセルで初期化
        public void MapInit(Rect mapRect, float tilesize = 0.0f)
        {
            if (tilesize != 0.0f) this.TileSize = tilesize;
            this.MapRect = mapRect;
            this.cellMapBody = new AstarCell[this.GridHeight * this.GridWidth];
            for (int y = 0; y < this.GridHeight; ++y)
            {
                for (int x = 0; x < this.GridWidth; ++x)
                {
                    var cell = new AstarCell();
                    cell.Position = new Vector2(x, y);
                    cellMapBody[cellIndex(x, y)] = cell;
                }
            }
        }

        public void Reset()
        {
            foreach (var cell in this.cellMapBody)
            {
                if (cell.CellType != AstarCell.Type.Start && cell.CellType != AstarCell.Type.Goal && cell.CellType != AstarCell.Type.Block)
                {
                    cell.Reset();
                }
            }
            this.pathfindFinished = false;
            this.first = true;
            this.pathList = null;
        }

        public void MapMake(bool optimizing)
        {
            foreach (var cell in this.cellMapBody)
            {
                if (cell.CellType != AstarCell.Type.Block)
                    cell.CellType = AstarCell.Type.Empty;
                if (!optimizing)
                {
                    setGridRelated(cell);
                }
                else
                {
                    cell.Related.Clear();
                }
            }

            // 基本的なAStartでは処理が冗長のため、最適化を行う
            if (optimizing)
            {
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
                foreach(var cell in this.cellMapBody.Where(c => c.CellType == AstarCell.Type.Empty))
                {
                    var pos = cell.Position;
                    AstarCell.Type[,] ar = new AstarCell.Type[3,3]; // check around
                    for (int ix = -1; ix < 2; ++ix)
                    {
                        for (int iy = -1; iy < 2; ++iy)
                        {
                            if (ix == 0 && iy == 0) continue;
                            float x = pos.x + (ix * this.TileSize);
                            float y = pos.y + (iy * this.TileSize);
                            ar[ix+1,iy+1] = this.CellType(x, y);
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
                                    ||  (m(1, 0) == AstarCell.Type.Block && m(u, 0) == AstarCell.Type.Block && m(u, 2) != AstarCell.Type.Block))
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
            }
        }

        public void PathFind(Vector2 start, Vector2 goal, System.Action<List<Vector2>> onEnd = null, ExecuteMode mode = ExecuteMode.ASync)
        {
            this.StartPoint = start;
            this.GoalPoint = goal;
            this.CellMap(start.x, start.y).CellType = AstarCell.Type.Start;
            this.CellMap(goal.x, goal.y).CellType = AstarCell.Type.Goal;
            this.pathCount = 0;

            if (this.first)
            {   // スタートマスの回りをスキャン
                var startCell = this.cellMapBody.FirstOrDefault(c => c.CellType == AstarCell.Type.Start);
                ScanAround(startCell);
                this.first = false;
            }
            switch(mode)
            {
                case ExecuteMode.ASync:
                    StartCoroutine(pathFindProcessCoroutine(onEnd));
                    break;
                case ExecuteMode.Sync:
                    while(!this.pathfindFinished) 
                    {
                        pathFindProcess();
                    }
                    if (onEnd != null) onEnd(this.pathList);
                    break;
                default:
                    pathFindProcess();
                    if (this.pathfindFinished && onEnd != null)
                    {
                        onEnd(this.pathList);
                    }
                    break;
            }
        }

        private IEnumerator pathFindProcessCoroutine(System.Action<List<Vector2>> onEnd)
        {
            while(!this.pathfindFinished)
            {
                pathFindProcess();
                yield return null;
            }
            if (onEnd != null) onEnd(this.pathList);
        }
        private void pathFindProcess()
        {
            ++this.pathCount;
            var cells = this.cellMapBody.Where(c => c.CellType == AstarCell.Type.Open)
                        .OrderBy(c => c.Score)
                        .ThenBy(c => c.Cost);

            if (cells.Any())
            {
                var score = cells.ElementAt(0).Score;
                foreach (var cell in cells.Where(c => c.Score == score))
                {
                    ScanAround(cell);
                    if (this.pathfindFinished) break;
                    cell.CellType = AstarCell.Type.Close;
                }
            }
            else
            {   // 解決不能
                this.pathfindFinished = true;
            }
        }

        List<Vector2> pathList;
        private void Goal(AstarCell cell)
        {
            this.pathList = new List<Vector2>();
            this.pathList.Add(this.GoalPoint);
            var parent = cell;
            while (parent.Parent != null)
            {
                this.pathList.Add(parent.Position);
                parent.CellType = AstarCell.Type.Correct;
                parent = parent.Parent;
            }
            this.pathList.Add(this.StartPoint);
            this.pathList.Reverse();
            if (this.GridMode)
            {
                this.pathList = fillGrid(this.pathList);
            }
            this.pathfindFinished = true;
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
                      result.Add(new Vector2(x,y));
                      return false;
                  });
            }
            return result;
        }


        private void ScanAround(AstarCell parent)
        {
            if (parent.Related.Count == 0)
            {   // 接続情報がない場合は作成する
                setGridRelatedSearchRaycast(parent);
            }
            foreach(var related in parent.Related)
            { 
                if (related.cell.CellType == AstarCell.Type.Goal)
                {   // !! GOAL!
                    Goal(parent);
                    return;
                }

                //float cost = parent.Cost + (related.cell.Position - parent.Position).magnitude;
                float cost = parent.Cost + related.cost;
                float hint = (this.GoalPoint - related.cell.Position).magnitude;
                float score = cost + hint;
                if (related.cell.CellType == AstarCell.Type.Empty || related.cell.Score > score)
                {
                    related.cell.Cost = cost;
                    related.cell.Hint = hint;
                    related.cell.Score = score;
                    related.cell.Parent = parent;
                    related.cell.CellType = AstarCell.Type.Open;
                }
            }
        }

        // 上下左右斜めのグリッドに対してリンクを作成する
        private void setGridRelated(AstarCell parent)
        {
            parent.Related.Clear();
            if (parent.CellType == AstarCell.Type.Block) return;
            int x = (int)parent.Position.x;
            int y = (int)parent.Position.y;
            for (int dx = -1; dx < 2; ++dx)
            {
                for (int dy = -1; dy < 2; ++dy)
                {
                    if (dx == 0 && dy == 0) continue;
                    float nx = x + dx * this.TileSize;
                    float ny = y + dy * this.TileSize;
                    Vector2 n = new Vector2(nx, ny);
                    if (!this.MapRect.Contains(n)) continue;
                    var cell = this.CellMap(nx, ny);
                    if (cell != null && cell.CellType != AstarCell.Type.Block)
                    {
                        parent.AddRelated(this.CellMap(nx, ny), (cell.Position - parent.Position).magnitude);
                    }
                }
            }
        }

        // 到達可能なノードを全ノードからレイキャストして調べる
        public void setGridRelatedSearchRaycast(AstarCell parent)
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

        // 全方位に有効なセルをスキャンして接続先とする
        private void setGridRelatedSearchAround(AstarCell parent)
        {
            parent.ClearRelated();

            float[,] dxy = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { -1, -1 } };
            float x = 0.0f;
            float y = 0.0f;
            for (int step = 0; step < 4; ++step)
            {
                if (step == 1) { x = this.MapRect.xMax - this.TileSize; y = 0.0f; }
                else if (step == 2) { x = this.MapRect.xMax - this.TileSize; y = this.MapRect.yMax - this.TileSize; }
                else if (step == 3) { x = 0.0f; y = this.TileSize; y = this.MapRect.yMax - this.TileSize; }
                for (int i = 0; i < ((step == 0 || step == 2) ? this.GridWidth : this.GridHeight); ++i)
                {
                    // raycast
                    RaycastCell(parent.Position, new Vector2(x, y),
                        (cx, cy) =>
                        {
                            var type = this.CellType(cx, cy);
                            if (type != AstarCell.Type.Removed)
                            {
                                if (type == AstarCell.Type.Empty)
                                {
                                    var rcell = this.CellMap(cx, cy);
                                    if (rcell.CellType == AstarCell.Type.Empty && !parent.Contains(rcell))
                                    {
                                        parent.AddRelated(rcell, (new Vector2(x, y) - parent.Position).magnitude);
                                    }
                                }
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        });
                    x += dxy[step,0] * this.TileSize;
                    y += dxy[step,1] * this.TileSize;
                }
            }
        }

        // DDAでレイキャスト
        // (src,target] までDDAでトレースする
        public void RaycastCell(Vector2 src, Vector2 target, System.Func<float,float, bool> act)
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
    }
}

