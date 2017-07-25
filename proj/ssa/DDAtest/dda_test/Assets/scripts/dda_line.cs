using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class dda_line : MonoBehaviour {

    public int x1,y1,x2,y2;
    public UnityEngine.UI.Text Text;

    public SceneBehaviour sceneBehabiour;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        if (Input.GetMouseButton(0))
        {
            var mpos = Input.mousePosition;
            var pos = Camera.main.ScreenToWorldPoint(mpos);
            int px = (int)(pos.x * 16/ 50 + 16);
            int py = (int)(pos.y * 16/ 50 + 16);
            if (px != x2 || py != y2)
            {
                x2 = px;
                y2 = py;
                Clear();
                OnStartDrawLine();
            }
        }
		
	}

    public void Clear()
    {
        this.sceneBehabiour.Clear();
    }

    public void OnStartDrawLine()
    {
        line(x1,y1,x2, y2, Color.red);
    }

    void line(int x1, int y1, int x2, int y2, Color col)
    {
        int dx = x2 - x1;
        int dy = y2 - y1;

        this.Text.text = string.Format("({0},{1})-({2},{3}) dx:{4} dy:{5}", x1, y1, x2, y2, dx, dy);

        if (dx > dy)
        {   // Xの増分が大きいので、Xを1ずつ増やしていく
            int r = (dx - dy) / 2;
            int y = y1;
            for (int x = x1; x <= x2; ++x)
            {
                this.sceneBehabiour.SetPixel(x, y, col);
                r -= dy;
                if (r < 0)
                {
                    r += dx;
                    ++y;
                }
            }
        }
        else
        {   // Yの増分が大きいので、Yを1ずつ増やしていく
            int r = (dy - dx) / 2;
            int x = x1;
            for (int y = y1; y <= y2; ++y)
            {
                this.sceneBehabiour.SetPixel(x, y, col);
                r -= dx;
                if (r < 0)
                {
                    r += dy;
                    ++x;
                }
            }
        }
    }
}
