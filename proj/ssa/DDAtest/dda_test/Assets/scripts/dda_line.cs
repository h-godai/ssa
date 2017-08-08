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


    //  線の方程式
    //  y = a * x + b
    //  これを微分する
    //  y' = ?
    //  デジタル微分では、xが+1したときに、yがどれだけ増えるかを調べる
    //  y' = (a * (x + 1) + b) - (a * x + b)
    //  y' = a * x + a - a * x
    //  y' = a
    //  つまり、y = a * x + bを微分するとaになる
    //  xが1増えると、yはaだけ増える 
    //
    //  (x1, y1) - (x2, y2) の2点を結ぶ直線を考える
    //   dx = x2 - x1
    //   dy = y2 - y1
    //   a = dy / dx
    //  
    //   直線を引くとき、dxとdyの大きさから傾きが45度以上かどうかわかる
    //   1ドットずつ点を描くために、aの値が1以下になるように計算する
    //   dx / dy の計算を引き算のループで表現する
    //   r = dx
    //   while
    //   {
    //     r -= dy
    //     if (r < 0) r += dx // このとき、yが一つ増える 
    //   }
    //   x1とy1を代入して、x2までのループにする
    //   r = dx
    //   for (x = x1; x <= x2; ++x)
    //   {
    //      r -= dy
    //      if (r < 0) { r += dx; ++y; }
    //    }
    //


    void line(int x1, int y1, int x2, int y2, Color col)
    {
        int dx = x2 - x1;
        int dy = y2 - y1;

        this.Text.text = string.Format("({0},{1})-({2},{3}) dx:{4} dy:{5}", x1, y1, x2, y2, dx, dy);

        if (dx > dy)
        {   // Xの増分が大きいので、Xを1ずつ増やしていく
            int r = dx; //  (dx - dy) / 2;
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
