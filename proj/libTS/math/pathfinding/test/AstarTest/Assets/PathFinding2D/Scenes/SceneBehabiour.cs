using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using AStarPathfinder2D = Tsl.Math.Pathfinder.AStarPathfinder2D;
using AStarPathfinder2DBasic = Tsl.Math.Pathfinder.AStarPathfinder2DBasic;

using AstarCell = Tsl.Math.Pathfinder.AstarCell;

public class SceneBehabiour : MonoBehaviour {

    public Transform MapRoot;
    public GameObject CellPrefab;
    public int GridWidth = 16;
    public int GridHeight = 16;
    public Vector2 StartPoint = new Vector2(0, 0);
    public Vector2 GoalPoint = new Vector2(15, 15);
    public UnityEngine.UI.Text MessageText;
    public UnityEngine.UI.Text TestText;

    private bool goled = false;

    private Tsl.UI.Pathfinder.Cell[,] cellMap;
    private float distance = 0.0f;

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
        AStarPathfinder2DBasic.Instance.MapInit(AStarPathfinder2D.Instance);
    }

    private void Update()
    {
        this.MessageText.text = string.Format("{0} Nodes\n{1} Links\n{2} Paths\ndistance={3}",
            AStarPathfinder2D.Instance.NumOfNodes,
            AStarPathfinder2D.Instance.NumOfLinks,
            AStarPathfinder2D.Instance.PathCount,
            this.distance);

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
        this.distance = 0.0f;
        if (lines != null)
        {
            for (int i = 0; i < lines.Count - 1; ++i)
            {
                this.distance += (lines[i+1] - lines[i]).magnitude;
                if (!AStarPathfinder2D.Instance.GridMode)
                {
                    AStarPathfinder2D.Instance.RaycastCell(lines[i], lines[i + 1], (x, y) =>
                      {
                          var cell = AStarPathfinder2D.Instance.CellMap(x, y);
                          if (cell.CellType == AstarCell.Type.Removed) cell.CellType = AstarCell.Type.SkipPoint;
                          return false;
                      });
                }
            }
        }
    }

    public void OnClickMapMakeButton(bool opt)
    {
        if (opt)
            AStarPathfinder2D.Instance.MapMake();
        else
            AStarPathfinder2DBasic.Instance.MapMake();
    }

    public void OnClickRandomMake()
    {
        for (int n = 0; n < 10; ++n)
        {
            int l = UnityEngine.Random.Range(1,10);
            int x = Random.Range(0, this.GridWidth);
            int y = Random.Range(0, this.GridHeight);
            bool dir = Random.Range(0,2) == 0;
            while(l-- != 0)
            {
                if (x < 0 || y < 0 || x >= this.GridWidth || y >= this.GridHeight) break;
                AStarPathfinder2D.Instance.CellMap(x, y).CellType = Tsl.Math.Pathfinder.AstarCell.Type.Block;
                x += dir ? 1 : 0;
                y += dir ? 0 : 1;
            }
        }
    }
    
    public void OnClickClear()
    {
        for (int y = 0; y < this.GridHeight; ++y)
        {
            for (int x = 0; x < this.GridWidth; ++x)
            {
                AStarPathfinder2D.Instance.CellMap(x, y).CellType = Tsl.Math.Pathfinder.AstarCell.Type.Removed;
            }
        }
    }

    public void OnClickAutoTest()
    {
        StartCoroutine(AutoTest());
    }
    private IEnumerator AutoTest()
    {
        double basicTime = 0.0;
        double optimizedTime = 0.0;
        int testCount = 0;
        for (int cnt = 0; cnt < 100; ++cnt)
        {
            OnClickClear();
            OnClickRandomMake();
            OnClickRandomMake();
            OnClickRandomMake();
            Reset();
            OnClickMapMakeButton(false);
            yield return null;

            this.StartPoint = new Vector2(Random.Range(0, this.GridWidth/3), Random.Range(0, this.GridHeight/3));
            this.GoalPoint = new Vector2(Random.Range(0, this.GridWidth/3) + this.GridWidth*2/3, Random.Range(0, this.GridHeight/3)+this.GridHeight*2/3);
            this.goled = false;
            var now = System.DateTime.Now;
            AStarPathfinder2D.Instance.PathFind(this.StartPoint, this.GoalPoint, r => 
            {
                basicTime += (System.DateTime.Now - now).TotalSeconds;
                DrawLine(r);
                this.goled = true;
            }, Tsl.Math.Pathfinder.AStarPathfinder2D.ExecuteMode.Sync);

            while (!this.goled) yield return null;
            yield return null;
            var dis = this.distance;
            Reset();
            this.goled = false;
            OnClickMapMakeButton(true);
            now = System.DateTime.Now;
            AStarPathfinder2D.Instance.PathFind(this.StartPoint, this.GoalPoint, r => 
            {
                optimizedTime += (System.DateTime.Now - now).TotalSeconds;
                DrawLine(r);
                this.goled = true;
            }, Tsl.Math.Pathfinder.AStarPathfinder2D.ExecuteMode.Sync);
            while (!this.goled) yield return null;
            yield return null;

            if (dis != 0.0f)
            {
                ++testCount;
                if (Mathf.Abs(dis - this.distance) > 0.01f)
                {
                    Debug.Log(string.Format("distance not equal opt:{0} as {1}", dis, this.distance));
                    break;
                }
            }

            TestText.text = string.Format("{0} tests\n       basic: {1:0.000} / {2:0.000}\noptimized: {3:0.000} / {4:0.000}",
                             testCount, basicTime, basicTime/testCount, optimizedTime,  optimizedTime/testCount);

            yield return null;
            Reset();
        }
    }
}
