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
    public UnityEngine.UI.Text MessageText;

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

    private void Update()
    {
        this.MessageText.text = string.Format("{0}Nodes\n{1}Links\n{2}Paths",
            AStarPathfinder2D.Instance.NumOfNodes,
            AStarPathfinder2D.Instance.NumOfLinks,
            AStarPathfinder2D.Instance.PathCount);

    }

    public void Reset()
    {
        AStarPathfinder2D.Instance.Reset();
        this.goled = false;
    }

    public void OnClickStartButton()
    {
        if (this.goled)
        {
            Reset();
        }
        else
        {
            AStarPathfinder2D.Instance.PathFind(this.StartPoint, this.GoalPoint, r => 
            {
                DrawLine(r);
                this.goled = true;
            });
        }
    }

    private void DrawLine(List<Vector2> lines)
    {
        for (int i = 0; i < lines.Count - 1; ++i)
        {
            AStarPathfinder2D.Instance.RaycastCell(lines[i], lines[i + 1], (x, y) =>
              {
                  var cell = AStarPathfinder2D.Instance.CellMap(x, y);
                  if (cell.CellType == AstarCell.Type.Removed) cell.CellType = AstarCell.Type.SkipPoint;
                  return false;
              });
        }
    }

    public void OnClickMapMakeButton(bool opt)
    {
        AStarPathfinder2D.Instance.MapMake(opt);
    }
}
