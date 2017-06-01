using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using AStarPathfinder2D = Tsl.Math.Pathfinder.AStarPathfinder2D;
using AstarCell = Tsl.Math.Pathfinder.AstarCell;

public class SceneBehabiour : MonoBehaviour {

    public Transform MapRoot;
    public GameObject CellPrefab;
    public int GridWidth = 16;
    public int GridHeight = 16;
    public Vector2 StartPoint = new Vector2(0, 0);
    public Vector2 GoalPoint = new Vector2(15, 15);

    private bool goled = false;

    private Tsl.UI.Pathfinder.Cell[,] cellMap;

    // Use this for initialization
    void Start () {
        AStarPathfinder2D.Instance.MapInit(new Rect(0,0, this.GridHeight, this.GridWidth));

        this.cellMap = new Tsl.UI.Pathfinder.Cell[this.GridHeight,this.GridWidth];
        for (int y = 0; y < this.GridHeight; ++y)
        {
            for (int x = 0; x < this.GridWidth; ++x)
            {
                var cell = Instantiate(CellPrefab.gameObject) as GameObject;
                this.cellMap[x, y] = cell.GetComponent<Tsl.UI.Pathfinder.Cell>();
                cell.transform.SetParent(this.MapRoot, false);
                this.cellMap[x, y].AstarCell = AStarPathfinder2D.Instance.CellMap(x, y);
            }

        }
    }

    public void OnClickStartButton()
    {
        if (this.goled)
        {
            AStarPathfinder2D.Instance.Reset();
            this.goled = false;
        }
        else
        {
            AStarPathfinder2D.Instance.PathFind(this.StartPoint, this.GoalPoint, r => { this.goled = true; });
        }
    }

    public void OnClickMapMakeButton(bool opt)
    {
        AStarPathfinder2D.Instance.MapMake(opt);
    }
}
