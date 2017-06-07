using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using AstarCell = Tsl.Math.Pathfinder.AstarCell;
using AstarCell2D = Tsl.Math.Pathfinder.AstarCell2D;

namespace Tsl.UI.Pathfinder
{
    public class Cell : MonoBehaviour
    {
        public Text Text;
        public Image Image;
        public AstarCell2D AstarCell;
        public Color[] CellColor = new Color[9];

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
            if (this.AstarCell != null)
            {
                this.Image.color = this.CellColor[(int)this.AstarCell.CellType];
                this.Text.text = string.Format("C:{0:0.00}\nH:{1:0.00}\nS:{2:0.00}\n",
                    this.AstarCell.Cost, this.AstarCell.Hint, this.AstarCell.Score);
            }
        }

        private void onClick()
        {
            if (this.CellType == AstarCell2D.Type.Empty || this.CellType ==  AstarCell2D.Type.Correct)
            {
                Tsl.Math.Pathfinder.AStarPathfinder2D.Instance.setGridRelatedSearchRaycast(this.AstarCell);
                var newtype = this.CellType == AstarCell2D.Type.Empty ? AstarCell2D.Type.Correct : AstarCell2D.Type.Empty;
                foreach(var cell in this.AstarCell.Related)
                {
                    cell.cell.CellType = newtype;
                }
                this.CellType = newtype;
            }
            else if (this.CellType == AstarCell2D.Type.Block) this.CellType = AstarCell2D.Type.Removed;
            else if (this.CellType == AstarCell2D.Type.Removed) this.CellType = AstarCell2D.Type.Block;
        }
    }

}

