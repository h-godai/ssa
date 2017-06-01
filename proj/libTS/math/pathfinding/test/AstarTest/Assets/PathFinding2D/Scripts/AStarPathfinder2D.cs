using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

namespace Tsl.Math.Pathfinder
{
    public class AStarPathfinder2D : MonoBehaviour
    {
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

        public float TileSize = 1.0f;
        private int GridWidth {  get {  return (int)(this.MapRect.width / this.TileSize);} }
        private int GridHeight {  get {  return (int)(this.MapRect.height / this.TileSize);} }

        private int cellIndex(float x, float y)
        {
            int ix = (int)((x - this.MapRect.x) / this.TileSize + 0.5f);
            int iy = (int)((y - this.MapRect.y) / this.TileSize + 0.5f);
            return iy * this.GridWidth + ix;
        }
        public AstarCell CellMap(float x, float y)
        {
            return cellMapBody[cellIndex(x, y)];
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
                setGridRelated(cell);
            }

            if (optimizing)
            {
                // 8方向全部リンクがあるセルを削除
                var octcells = this.cellMapBody.Where(c => c.Related.Count == 8);
                foreach (var cell in octcells)
                {
                    cell.CellType = AstarCell.Type.Removed;
                }
                var allcell = this.cellMapBody.Where(c => c.CellType != AstarCell.Type.Removed);
                foreach (var cell in allcell)
                {
                    setGridRelatedSearchLink(cell);
                }
            }
        }
        private void mergeLink(AstarCell src, AstarCell dst)
        {
            foreach(var r in src.Related)
            {
                if (r == src || r == dst) continue;
                if (!dst.Related.Contains(r)) dst.Related.Add(r);
            }
        }

        public void PathFind(Vector2 start, Vector2 goal, System.Action<List<Vector2>> onEnd = null, ExecuteMode mode = ExecuteMode.ASync)
        {
            this.StartPoint = start;
            this.GoalPoint = goal;
            this.CellMap(start.x, start.y).CellType = AstarCell.Type.Start;
            this.CellMap(goal.x, goal.y).CellType = AstarCell.Type.Goal;

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
            this.pathfindFinished = true;
            this.pathList = new List<Vector2>();
            var parent = cell;
            while (parent.Parent != null)
            {
                this.pathList.Add(parent.Position);
                parent.CellType = AstarCell.Type.Correct;
                parent = parent.Parent;
            }
            this.pathList.Reverse();
        }

        private void ScanAround(AstarCell parent)
        {
            foreach(var cell in parent.Related)
            { 
                if (cell.CellType == AstarCell.Type.Goal)
                {   // !! GOAL!
                    Goal(parent);
                    return;
                }

                float cost = parent.Cost + (cell.Position - parent.Position).magnitude;
                float hint = (this.GoalPoint - cell.Position).magnitude;
                float score = cost + hint;
                if (cell.CellType == AstarCell.Type.Empty || cell.Score > score)
                {
                    cell.Cost = cost;
                    cell.Hint = hint;
                    cell.Score = score;
                    cell.Parent = parent;
                    cell.CellType = AstarCell.Type.Open;
                }
            }
        }

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
                        parent.Related.Add(this.CellMap(nx, ny));
                    }
                }
            }
        }

        // 全方位に有効なセルをスキャンして接続先とする
        private void setGridRelatedSearchLink(AstarCell parent)
        {
            parent.Related.Clear();
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
                    var rcell = raycastCell(parent.Position, new Vector2(x, y));
                    if (rcell != null && rcell.CellType == AstarCell.Type.Empty && !parent.Related.Contains(rcell))
                    {
                        parent.Related.Add(rcell);
                    }
                    x += dxy[step,0] * this.TileSize;
                    y += dxy[step,1] * this.TileSize;
                }
            }
        }

        // DDAでレイキャスト
        private AstarCell raycastCell(Vector2 src, Vector2 target)
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
                    var cell = this.CellMap(x, y);
                    if (cell.CellType != AstarCell.Type.Removed) return cell;
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
                    var cell = this.CellMap(x, y);
                    if (cell.CellType != AstarCell.Type.Removed) return cell;
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
                    var cell = this.CellMap(x, y);
                    if (cell.CellType != AstarCell.Type.Removed) return cell;
                }
            }
            return null;
        }
    }
}

