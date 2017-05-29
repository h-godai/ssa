using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneBehabiour : MonoBehaviour {

    public Transform MapRoot;
    public GameObject CellPrefab;
    public int GridWidth = 16;
    public int GridHeight = 16;

    private PathFinding2D.UI.Cell[,] cellMap;

	// Use this for initialization
	void Start () {
        this.cellMap = new PathFinding2D.UI.Cell[this.GridHeight, this.GridWidth];
        for (int y = 0; y < this.GridHeight; ++y)
        {
            for (int x = 0; x < this.GridWidth; ++x)
            {
                var cell = Instantiate(CellPrefab.gameObject) as GameObject;
                this.cellMap[x, y] = cell.GetComponent<PathFinding2D.UI.Cell>();
                cell.transform.SetParent(this.MapRoot, false);
            }

        }
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
