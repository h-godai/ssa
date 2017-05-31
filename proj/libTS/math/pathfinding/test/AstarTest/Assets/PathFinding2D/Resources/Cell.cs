using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace PathFinding2D.UI
{
    public class Cell : MonoBehaviour
    {
        public Text Text;
        public Image Image;
        public AstarCell AstarCell = new AstarCell();
        public Color[] CellColor = new Color[5];

        public AstarCell.Type CellType
        {
            get { return this.AstarCell.CellType; }
            set { this.AstarCell.CellType = value; }
        }
        // Use this for initialization
        void Start()
        {
            var btn = GetComponent<Button>();
            btn.onClick.AddListener(() => { onClick(); });
        }

        // Update is called once per frame
        void Update()
        {
            this.Image.color = this.CellColor[(int)this.AstarCell.CellType];
            this.Text.text = string.Format("C:{0:0.00}\nH:{1:0.00}\nS:{2:0.00}\n",
                this.AstarCell.Cost, this.AstarCell.Hint, this.AstarCell.Score);
        }

        private void onClick()
        {
            if (this.CellType == AstarCell.Type.Block) this.CellType = AstarCell.Type.Empty;
            else if (this.CellType == AstarCell.Type.Empty) this.CellType = AstarCell.Type.Block;
        }
    }

}

namespace PathFinding2D
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
            Correct
        }
        public Vector2 Position;
        public Type CellType = Type.Empty;
        public float Score = 0.0f;
        public float Cost = 0.0f;
        public float Hint = 0.0f;
        public AstarCell Parent = null;

        public void Reset()
        {
            this.CellType = Type.Empty;
            this.Score = 0.0f;
            this.Cost = 0.0f;
            this.Hint = 0.0f;
            this.Parent = null;
        }
    }

}