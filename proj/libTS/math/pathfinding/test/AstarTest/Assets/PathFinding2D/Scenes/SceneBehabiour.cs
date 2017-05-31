using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

public class SceneBehabiour : MonoBehaviour {

    public Transform MapRoot;
    public GameObject CellPrefab;
    public int GridWidth = 16;
    public int GridHeight = 16;
    public Vector2 StartPoint = new Vector2(0, 0);
    public Vector2 GoalPoint = new Vector2(15, 15);

    private int StartX { get { return (int)this.StartPoint.x; } }
    private int StartY { get { return (int)this.StartPoint.y; } }
    private int GoalX { get { return (int)this.GoalPoint.x; } }
    private int GoalY { get { return (int)this.GoalPoint.y; } }
    private bool goled = false;
    private bool first = true;

    private PathFinding2D.UI.Cell[] cellMapBody;
    private PathFinding2D.UI.Cell cellMap(int x, int y)
    {
        return cellMapBody[y * this.GridWidth + x];
    }
    private void cellMap(int x, int y, PathFinding2D.UI.Cell cell)
    {
        cellMapBody[y * this.GridWidth + x] = cell;
    }

    // Use this for initialization
    void Start () {
        this.cellMapBody = new PathFinding2D.UI.Cell[this.GridHeight * this.GridWidth];
        for (int y = 0; y < this.GridHeight; ++y)
        {
            for (int x = 0; x < this.GridWidth; ++x)
            {
                var cell = Instantiate(CellPrefab.gameObject) as GameObject;
                this.cellMap(x, y, cell.GetComponent<PathFinding2D.UI.Cell>());
                cell.transform.SetParent(this.MapRoot, false);
                this.cellMap(x, y).AstarCell.Position = new Vector2(x, y);
            }

        }
        this.cellMap(this.StartX, this.StartY).AstarCell.CellType = PathFinding2D.AstarCell.Type.Start;
        this.cellMap(this.GoalX, this.GoalY).AstarCell.CellType = PathFinding2D.AstarCell.Type.Goal;
    }

    // Update is called once per frame
    void Update () {
		
	}

    public void PathFind()
    {
        if (this.first)
        {
            // スタートマスの回りをスキャン
            ScanAround(this.cellMap(this.StartX, this.StartY).AstarCell);
            this.first = false;
        }

        var cells = this.cellMapBody.Where(c => c.CellType == PathFinding2D.AstarCell.Type.Open)
                    .OrderBy(c => c.AstarCell.Score)
                    .ThenBy(c => c.AstarCell.Cost);

        if (cells.Any())
        {
            var score = cells.ElementAt(0).AstarCell.Score;
            foreach(var cell in cells.Where(c => c.AstarCell.Score == score))
            {
                ScanAround(cell.AstarCell);
                if (this.goled) break;
                cell.AstarCell.CellType = PathFinding2D.AstarCell.Type.Close;
            }
        }
        else
        {
            Reset();
        }
    }

    private void Goal(PathFinding2D.AstarCell cell)
    {
        this.goled = true;
        var parent = cell;
        while (parent.Parent != null)
        {
            parent.CellType = PathFinding2D.AstarCell.Type.Correct;
            parent = parent.Parent;
        }
    }

    private void ScanAround(PathFinding2D.AstarCell parent)
    {
        int x = (int)parent.Position.x;
        int y = (int)parent.Position.y;
        for (int dx = -1; dx < 2; ++dx)
        {
            for (int dy = -1; dy < 2; ++dy)
            {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (nx < 0 || nx >= this.GridWidth || ny < 0 || ny >= this.GridHeight) continue;
                var cell = this.cellMap(nx, ny).AstarCell;
                if (cell.CellType == PathFinding2D.AstarCell.Type.Goal)
                {   // !! GOAL!
                    Goal(parent);
                    return;
                }

                Vector2 n = new Vector2(nx, ny);
                float cost = parent.Cost + Mathf.Sqrt(dx * dx + dy * dy);
                float hint = (this.GoalPoint - n).magnitude;
                float score = cost + hint;
                if (cell.CellType == PathFinding2D.AstarCell.Type.Empty || cell.Score > score)
                {
                    cell.Cost = cost;
                    cell.Hint = hint;
                    cell.Score = score;
                    cell.Parent = parent;
                    cell.CellType = PathFinding2D.AstarCell.Type.Open;
                }
            }
        }
    }
    public void Reset()
    {
        foreach(var cell in this.cellMapBody)
        {
            if (cell.CellType != PathFinding2D.AstarCell.Type.Start && cell.CellType != PathFinding2D.AstarCell.Type.Goal)
            {
                cell.AstarCell.Reset();
            }
        }
        this.goled = false;
        this.first = true;
    }

    public void OnClickStartButton()
    {
        if (this.goled)
        {
            Reset();
        }
        else
        {
            PathFind();
        }
    }
}
