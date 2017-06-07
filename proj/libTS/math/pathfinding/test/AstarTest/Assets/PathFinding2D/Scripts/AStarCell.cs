using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

namespace Tsl.Math.Pathfinder
{
    public abstract class AstarCell
    {
        public enum Type
        {
            Empty,
            Start,
            Goal,
            Open,
            Close,
            Block,
            Correct,
            Removed,
            SkipPoint,
            Links,
        }

        public struct RelatedData 
        {
            public AstarCell cell;
            public float cost;
        }

        public Type CellType = Type.Removed;
        public float Score = 0.0f;
        public float Cost = 0.0f;
        public float Hint = 0.0f;
        // 親となるセル
        public AstarCell Parent = null;
        // 接続しているセル
        public List<RelatedData> Related = new List<RelatedData>();

        public abstract float Heuristic(AstarCell cell);

        public void Reset()
        {
            this.CellType = Type.Removed;
            this.Score = 0.0f;
            this.Cost = 0.0f;
            this.Hint = 0.0f;
            this.Parent = null;
            //this.Related.Clear();
        }

        public void ClearRelated()
        {
            this.Related.Clear();
        }
        public void AddRelated(AstarCell cell, float cost)
        {
            this.Related.Add(new RelatedData { cell = cell, cost = cost });
        }
        public bool Contains(AstarCell cell)
        {
            return this.Related.Any(r => r.cell == cell);
        }
        public RelatedData Find(AstarCell cell)
        {
            return this.Related.Find(r => r.cell == cell);
        }


        // 経路探索に有効なセルの場合true
        public bool IsValidCell()
        {
            return this.CellType == Type.Empty
                || this.CellType == Type.Open
                || this.CellType == Type.Close
                || this.CellType == Type.Goal
                || this.CellType == Type.Correct;
        }
    }

    public class AstarCell2D : AstarCell
    {
        public Vector2 Position;
        public override float Heuristic(AstarCell cell)
        {
            return ((cell as AstarCell2D).Position - this.Position).magnitude;
        }
    }
    public class AstarCell3D : AstarCell
    {
        public Vector3 Position;
        public override float Heuristic(AstarCell cell)
        {
            return ((cell as AstarCell3D).Position - this.Position).magnitude;
        }
    }

}