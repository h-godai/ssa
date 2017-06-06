using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

namespace Tsl.Math.Pathfinder
{
    public class AStarPathfinder2Dbase : MonoBehaviour
    {
        public float TileSize = 1.0f;
        protected AstarCell[] cellMapBody;
        protected Rect MapRect = new Rect(0, 0, 16, 16);
        protected List<Vector2> pathList; // 結果を一時的に保存する

        public enum ExecuteMode
        {
            Sync, // 最後までノンストップ
            ASync, // Coroutineで実行
            StepFirst, // 1ステップずつ
            StepNext, // 1ステップずつ
        }

        private int GridWidth { get { return (int)(this.MapRect.width / this.TileSize); } }
        private int GridHeight { get { return (int)(this.MapRect.height / this.TileSize); } }

        private int cellIndex(float x, float y)
        {
            int ix = (int)((x - this.MapRect.x) / this.TileSize + 0.5f);
            int iy = (int)((y - this.MapRect.y) / this.TileSize + 0.5f);
            if (x < this.MapRect.x || y < this.MapRect.y) return -1;
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

        // 動的なセルの追加
        public virtual void MakeRelation(AstarCell cell) { throw new System.NotImplementedException(); }

        public int NumOfNodes
        {
            get { return this.cellMapBody.Where(c => c.CellType != AstarCell.Type.Removed && c.CellType != AstarCell.Type.Block).Count(); }
        }
        public int NumOfLinks
        {
            get { return this.cellMapBody.Where(c => c.CellType != AstarCell.Type.Removed && c.CellType != AstarCell.Type.Block).Select(c => c.Related.Count).Sum(); }
        }

        public int PathCount
        {
            get { return this.finder == null ? 0 : this.finder.PathCount; }
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
        // srcのマップをコピー
        public void MapInit(AStarPathfinder2Dbase src)
        {
            this.TileSize = src.TileSize;
            this.MapRect = src.MapRect;
            this.cellMapBody = src.cellMapBody;
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
        }

        protected virtual void Goal(AstarCell cell)
        {
            this.pathList = new List<Vector2>();
            this.pathList.Add(this.finder.goalCell.Position);
            var parent = cell;
            while (parent.Parent != null)
            {
                this.pathList.Add(parent.Position);
                parent.CellType = AstarCell.Type.Correct;
                parent = parent.Parent;
            }
            this.pathList.Add(this.finder.startCell.Position);
            this.pathList.Reverse();
        }

        public void PathFind(Vector2 start, Vector2 goal, System.Action<List<Vector2>> onEnd = null, ExecuteMode mode = ExecuteMode.ASync)
        {
            var startCell = this.CellMap(start.x, start.y);
            var goalCell = this.CellMap(goal.x, goal.y);
            startCell.CellType = AstarCell.Type.Start;
            goalCell.CellType = AstarCell.Type.Goal;

            if (mode != ExecuteMode.StepNext)
            {
                this.finder = new AStarPathfind2D();
                this.finder.PathFind(this.cellMapBody, this.MakeRelation, this.Goal);
            }
            switch(mode)
            {
                case ExecuteMode.ASync:
                    StartCoroutine(pathFindProcessCoroutine(onEnd));
                    break;
                case ExecuteMode.Sync:
                    while(!this.finder.Finished) 
                    {
                        this.finder.pathFindProcess();
                    }
                    if (onEnd != null) onEnd(this.pathList);
                    break;
                default:
                    this.finder.pathFindProcess();
                    if (this.finder.Finished && onEnd != null)
                    {
                        onEnd(this.pathList);
                    }
                    break;
            }
        }
        private IEnumerator pathFindProcessCoroutine(System.Action<List<Vector2>> onEnd)
        {
            while(!this.finder.Finished)
            {
                this.finder.pathFindProcess();
                yield return null;
            }
            if (onEnd != null) onEnd(this.pathList);
        }

        AStarPathfind2D finder;
    }

    public class AStarPathfind2D
    { 
        int pathCount = 0;
        bool pathfindFinished = false;
        System.Action<AstarCell> MakeRelation;
        System.Action<AstarCell> Goal;
        AstarCell[] cellMapBody;
        public AstarCell startCell;
        public AstarCell goalCell;

        public bool Finished {  get {  return this.pathfindFinished; } }
        public int PathCount {  get {  return this.pathCount; } }

        public void PathFind(AstarCell[] cellMapBody, System.Action<AstarCell> makeRelation, System.Action<AstarCell> goal)
        {
            this.startCell = cellMapBody.First(c => c.CellType == AstarCell.Type.Start);
            this.goalCell = cellMapBody.First(c => c.CellType == AstarCell.Type.Goal);
            this.cellMapBody = cellMapBody;
            this.MakeRelation = makeRelation;
            this.Goal = goal;
            this.pathCount = 0;
            this.pathfindFinished = false;
            this.goalCandidate.Clear();

            // スタートマスの回りをスキャン
            MakeRelation(this.goalCell);
            MakeRelation(this.startCell);
            ScanAround(this.startCell);
        }

        public void pathFindProcess()
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
                    cell.CellType = AstarCell.Type.Close;
                }
            }
            else
            {   // 解決不能
                this.pathfindFinished = true;
            }
            if (this.goalCandidate.Any())
            {   // goalしたものがいる場合、open cellでgoalよりスコアが良いものが無いか探す
                var goal = this.goalCandidate.OrderBy(g => g.Key).ElementAt(0);
                if (!this.cellMapBody.Any(c => c.CellType == AstarCell.Type.Open && c.Score < goal.Key))
                {
                    Goal(goal.Value);
                    this.pathfindFinished = true;
                }
            }
        }


        Dictionary<float, AstarCell> goalCandidate = new Dictionary<float, AstarCell>();

        private void ScanAround(AstarCell parent)
        {
            if (parent.Related.Count == 0)
            {   // 接続情報がない場合は作成する
                MakeRelation(parent);
            }
            foreach(var related in parent.Related)
            { 
                if (related.cell.CellType == AstarCell.Type.Goal)
                {   // !! GOAL!
                    var goalcost = parent.Cost + related.cost;
                    related.cell.Cost = goalcost;
                    related.cell.Score = goalcost;
                    if (this.goalCandidate.ContainsKey(goalcost))
                    {
                        this.goalCandidate[goalcost] = parent;
                    }
                    else
                    {
                        this.goalCandidate.Add(goalcost, parent);
                    }
                }
                float cost = parent.Cost + related.cost;
                float hint = (this.goalCell.Position - related.cell.Position).magnitude;
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
    }
}
