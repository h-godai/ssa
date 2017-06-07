using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

namespace Tsl.Math.Pathfinder
{

    public class AStarPathfindLogic
    { 
        int pathCount = 0;
        bool pathfindFinished = false;
        System.Action<AstarCell> MakeRelation;
        System.Action<AstarCell> Goal;
        public List<AstarCell> cells;
        public AstarCell startCell;
        public AstarCell goalCell;

        public bool Finished {  get {  return this.pathfindFinished; } }
        public int PathCount {  get {  return this.pathCount; } }

        public void PathFind(System.Action<AstarCell> makeRelation, System.Action<AstarCell> goal)
        {
            this.startCell = cells.First(c => c.CellType == AstarCell.Type.Start);
            this.goalCell = cells.First(c => c.CellType == AstarCell.Type.Goal);
            this.MakeRelation = makeRelation;
            this.Goal = goal;
            this.pathCount = 0;
            this.pathfindFinished = false;
            this.goalCandidate.Clear();

            // スタートマスの回りをスキャン
            this.MakeRelation(this.goalCell);
            this.MakeRelation(this.startCell);

            ScanAround(this.startCell);
        }

        public void pathFindProcess()
        {
            ++this.pathCount;
            var cells = this.cells.Where(c => c.CellType == AstarCell.Type.Open)
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
                if (!this.cells.Any(c => c.CellType == AstarCell.Type.Open && c.Score < goal.Key))
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
                float hint = this.goalCell.Heuristic(related.cell);
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
