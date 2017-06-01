using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Tsl.Math.Pathfinder
{
    public class AstarCell
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
        }
        public Vector2 Position;
        public Type CellType = Type.Removed;
        public float Score = 0.0f;
        public float Cost = 0.0f;
        public float Hint = 0.0f;
        // 親となるセル
        public AstarCell Parent = null;
        // 接続しているセル
        public List<AstarCell> Related = new List<AstarCell>();
        public void Reset()
        {
            this.CellType = Type.Removed;
            this.Score = 0.0f;
            this.Cost = 0.0f;
            this.Hint = 0.0f;
            this.Parent = null;
        }
    }

}