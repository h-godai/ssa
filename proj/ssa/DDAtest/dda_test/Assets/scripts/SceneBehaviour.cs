using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneBehaviour : MonoBehaviour {

    public Transform BaseBack;
    public int xMax = 32;
    public int yMax = 32;
    public int CenterX = 16;
    public int CenterY = 16;
    public TextMesh Xmax;
    public TextMesh Ymax;
    public Material meshMaterial;
    public Material redMaterial;

	// Use this for initialization
	void Start () {
		this.Xmax.text = xMax.ToString();
        this.Ymax.text = yMax.ToString();
        createMeth();
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    GameObject[,] grid;

    private void createMeth()
    {
        int w = (xMax - CenterX) * 2;
        int h = (yMax - CenterY) * 2;
        this.grid = new GameObject[w,h];
        float ofsx = -50.0f + 50.0f / w;
        float ofsy = -50.0f + 50.0f / h;
        for (int y = 0; y < h; ++y)
        {
            float py = ofsy + 100.0f * (float)y / (float)h;
            for (int x = 0; x < w; ++x)
            {
                float px = ofsx + 100.0f * (float)x / (float)w;
                GameObject quad = GameObject.CreatePrimitive(PrimitiveType.Quad);
                quad.transform.SetParent(this.transform, false);
                quad.transform.position = new Vector3(px, py, 0.0f);
                quad.transform.localScale = new Vector3(90.0f/w, 90.0f/h,1.0f);
                quad.GetComponent<MeshRenderer>().material = this.meshMaterial;
                grid[x,y] = quad;
            }
        }
    }

    public void Clear()
    {
        int w = (xMax - CenterX) * 2;
        int h = (yMax - CenterY) * 2;
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                SetPixel(x,y);
            }
        }
    }

    public void SetPixel(int x, int y, Color? col = null)
    {
        var newMaterial = new Material(this.meshMaterial);
        if (col != null)
        {
            newMaterial.color = col.Value;
            newMaterial.SetColor("_EmissionColor", col.Value);
        }
        this.grid[x,y].GetComponent<MeshRenderer>().material = newMaterial;

    }
}
