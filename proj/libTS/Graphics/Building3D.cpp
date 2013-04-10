#include "stdafx.h"
#include "Building3D.hpp"

namespace ts { namespace gobj {

  // ビルディング (底のないキューブ)
  //
  //  0  2  4  6    8(0)             5+----+7
  //  +--+--+--+---+                ／|  ／|
  //  |／|／|／|／/|              3+----+1 |
  //  +--+--+--++--+10(1)          | 4+-|--+6    z   y
  //  1  3  5 79|／|               |／  |／      | ／  
  //          11+--+12(3)          +----+          →x
  //                               2    0
  //  8=0,9=7,10=1,11=5,12=3
  // 
  // ひとつのTriangleStripで表現する
  // 上面 1-7-5-3  前面 0-1-3-2 右面 6-7-1-0  裏面 4-5-7-6 左面 2-3-5-4
  // 
  // テクスチャは前後、左右で共有する
  // 12           
  // +----+11                  -- 0.0
  // | T  |    
  // +----+----+----+----+     -- 0.3125
  // |10 9|7   |5   |3   |1
  // | S  | F  | S  | F  |
  // |    |    |    |    |
  // +----+----+----+----+     -- 1.0
  //  8    6    4    2    0
  //      |         |
  // 0.0  0.5  1.0  1.5   2.0
  //

  const uint32_t Building3D::buildingIndics[13] = {0,1,2,3, 4,5,6,7, 0,7,1,5,3};

  const float32_t   Building3D::sepY = 0.3125f;
  const TexCoords Building3D::buildingTexCoord[13] = {
    TexCoords( 2.0f, 1.0f ), // 0
    TexCoords( 2.0f, sepY ), // 1
    TexCoords( 1.5f, 1.0f ), // 2
    TexCoords( 1.5f, sepY ), // 3
    TexCoords( 1.0f, 1.0f ), // 4
    TexCoords( 1.0f, sepY ), // 5
    TexCoords( 0.5f, 1.0f ), // 6
    TexCoords( 0.5f, sepY ), // 7
    TexCoords( 0.0f, 1.0f ), // 8
    TexCoords( 0.5f, sepY ), // 9
    TexCoords( 0.0f, sepY ), // 10
    TexCoords( 0.5f, 0.0f ), // 11
    TexCoords( 0.0f, 0.0f )  // 12
  };
  const Vector3D Building3D::buildingVertex[8] = {
    Vector3D( 1.0f, -1.0f, 0.0f ), // 0
    Vector3D( 1.0f, -1.0f, 1.0f ), // 1
    Vector3D(-1.0f, -1.0f, 0.0f ), // 2
    Vector3D(-1.0f, -1.0f, 1.0f ), // 3
    Vector3D(-1.0f,  1.0f, 0.0f ), // 4
    Vector3D(-1.0f,  1.0f, 1.0f ), // 5
    Vector3D( 1.0f,  1.0f, 0.0f ), // 6
    Vector3D( 1.0f,  1.0f, 1.0f ), // 7
  };

  const uint32_t Building3D::rectIndics[6][4] = {
    {0,1,3,2}, // 正面
    {2,3,5,4}, // 左面
    {4,5,7,6}, // 裏面
    {6,7,1,0}, // 右面
    {1,7,5,3}  // 上面
  };

}}

