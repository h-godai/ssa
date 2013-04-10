// Building3D
//
// 底辺のないキューブ


#ifndef LIBTS_GRAPHICS_BUILDING3D_H
#define LIBTS_GRAPHICS_BUILDING3D_H

#include "GraphicsObject.hpp"

using namespace std;
using namespace boost;

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
#if 0
  // class内で配列の定数宣言ができないため
  namespace {
    const uint32_t buildingIndics[13] = {0,1,2,3, 4,5,6,7, 0,7,1,5,3};
    const float32_t sepY = 0.3125f;
    const TexCoords buildingTexCoord[13] = {
      { 2.0f, 1.0f }, // 0
      { 2.0f, sepY }, // 1
      { 1.5f, 1.0f }, // 2
      { 1.5f, sepY }, // 3
      { 1.0f, 1.0f }, // 4
      { 1.0f, sepY }, // 5
      { 0.5f, 1.0f }, // 6
      { 0.5f, sepY }, // 7
      { 0.0f, 1.0f }, // 8
      { 0.5f, sepY }, // 9
      { 0.0f, sepY }, // 10
      { 0.5f, 0.0f }, // 11
      { 0.0f, 0.0f }  // 12
    };
    const Vector3D buildingVertex[8] = {
      Vector3D( 1.0f, -1.0f, 0.0f ), // 0
      Vector3D( 1.0f, -1.0f, 1.0f ), // 1
      Vector3D(-1.0f, -1.0f, 0.0f ), // 2
      Vector3D(-1.0f, -1.0f, 1.0f ), // 3
      Vector3D(-1.0f,  1.0f, 0.0f ), // 4
      Vector3D(-1.0f,  1.0f, 1.0f ), // 5
      Vector3D( 1.0f,  1.0f, 0.0f ), // 6
      Vector3D( 1.0f,  1.0f, 1.0f ), // 7
    };
  }
#endif

  class Building3D : public Node {
    static const size_t IndicsCount = 13; // TriangleStripの頂点数
    static const size_t VertexCount = 8;  // 実際の頂点数
  public:
    typedef shared_ptr<Building3D> ptr;
    Building3D(float64_t x = 0.0, float64_t y = 0.0) 
      : m_color(1.0f,1.0f,1.0f,1.0f)
      , m_origin(Vector3Dd(x, y, 0.0))
      , m_scale(Vector3D(20.0f, 20.0f, 70.0f)) // 20x20x70mの初期値
      , m_direction(-22.5)
      , m_lineWidth(1.0f)
      , m_drawBorder(true)
      , m_drawSolid(true)
    {
      calcVertex();
    }
    virtual ~Building3D() {}
    void calcVertex() {
      Matrix matrix;
      matrix.translate(m_origin);
      matrix.rotate(m_direction, Vector3D(0.0f,0.0f,1.0f));
      matrix.scale(m_scale);
      for (size_t n = 0; n < VertexCount; ++n) {
        m_vertex[n] = matrix.mult(buildingVertex[n]);
      }
    }
    void calcVertex64(Vector3Dd* vtx) {
      Matrixd matrix;
      matrix.translate(m_origin);
      matrix.rotate(m_direction, Vector3Dd(0.0f,0.0f,1.0f));
      matrix.scale(m_scale);
      for (size_t n = 0; n < VertexCount; ++n) {
        vtx[n] = matrix.mult(buildingVertex[n]);
      }
    }

    void setTexture2D(shared_ptr<Texture2D> texture) {
      m_texture = texture;
      m_drawSolid = true;
      m_drawBorder = false;
      if (texture.get() != 0) {
        m_texture->setColor(m_color);
      }
    }

    Texture2D::ptr getTexture2D() {
      return m_texture;
    }


    void setColor(const ColorRGBA& col) { 
      m_color = col; 
      if (m_texture.get() != 0) {
         m_texture->setColor(m_color);
      }
    }
 
    virtual void doRendering(IRenderer* gl) {
      if (m_drawSolid) {
        // 面を書く
        if (m_texture.get() != 0) {
          // テクスチャあり
          gl->texture2D(*m_texture, m_texture->blendColor());
          beginPrimitive begin(gl, IRenderer::TRIANGLE_STRIP);
          for (size_t n = 0; n < IndicsCount; ++n) {
            gl->texcoord(buildingTexCoord[n]);
            gl->vertex(m_vertex[buildingIndics[n]]);
          }
        }
        else {
          // テクスチャなし
          gl->color(m_color);
          gl->disableTexture2D();
          beginPrimitive begin(gl, IRenderer::TRIANGLE_STRIP);
          for (size_t n = 0; n < IndicsCount; ++n) {
            gl->color(m_color);
            gl->vertex(m_vertex[buildingIndics[n]]);
          }
        }
      }
      if (m_drawBorder) {
        // ラインで四角形を描画
        gl->color(m_color);
        gl->disableTexture2D();
        gl->lineWidth(m_lineWidth);
        gl->begin(IRenderer::LINE_LOOP);
        gl->vertex(m_vertex[1]);
        gl->vertex(m_vertex[3]);
        gl->vertex(m_vertex[5]);
        gl->vertex(m_vertex[7]);
        gl->end();
        gl->begin(IRenderer::LINE_LOOP);
        gl->vertex(m_vertex[0]);
        gl->vertex(m_vertex[2]);
        gl->vertex(m_vertex[4]);
        gl->vertex(m_vertex[6]);
        gl->end();
        gl->begin(IRenderer::LINE);
        gl->vertex(m_vertex[0]);
        gl->vertex(m_vertex[1]);
        gl->vertex(m_vertex[2]);
        gl->vertex(m_vertex[3]);
        gl->vertex(m_vertex[4]);
        gl->vertex(m_vertex[5]);
        gl->vertex(m_vertex[6]);
        gl->vertex(m_vertex[7]);
        gl->end();

      }
    }

    // flag control
    void enableSolid(bool enable = true) { m_drawSolid = enable; }
    void enableBorder(bool enable = true) { m_drawBorder = enable; }

    // setter/getter
    void setScale(const Vector3D& scale) { m_scale = scale; }
    void setDirection(float32_t dir) { m_direction = dir; }
    void setOrigin(const Vector3Dd& origin) { m_origin = origin; }
    const Vector3D& getScale() const { return m_scale; }
    float32_t getDirection() const { return m_direction; }
    const Vector3Dd& getOrigin() { return m_origin; }
    void setLineWidth(float32_t lw) { m_lineWidth = lw; }
    Vector3D& getVertex(size_t n) { return m_vertex[n]; }
    Vector3D* getVertexes() { return &m_vertex[0]; }


    virtual void serialize(bstream& bs) {
      bs << static_cast<uint32_t>(sizeof(Building3D));
      bs << m_vertex;
      bs << m_normal;
      bs << m_color;
      bs << m_origin;
      bs << m_scale;
      bs << m_direction;
      bs << m_lineWidth;
      bs << m_drawSolid;
      bs << m_drawBorder;
    }
    virtual void deserialize(const bstream& bs) {
      uint32_t sz;
      bs >> sz;
      if (sz != sizeof(Building3D)) {
        // 読み込み失敗
//        throw GObjException("Building3D::deserialize");
      }
      bs >> m_vertex;
      bs >> m_normal;
      bs >> m_color;
      bs >> m_origin;
      bs >> m_scale;
      bs >> m_direction;
      bs >> m_lineWidth;
      bs >> m_drawSolid;
      bs >> m_drawBorder;
    }
      
      

    enum VertexPosition {
      FRB = 0, // front right bottom
      FRT, // front right top
      FLB, // front right bottom
      FLT, // ftont left top
      BLB,
      BLT,
      BRB,
      BRT
    };

    enum RectSurface {
      FRONT_RECT = 0,
      LEFT_RECT,
      BACK_RECT,
      RIGHT_RECT,
      TOP_RECT
    };

    static const uint32_t buildingIndics[13];
    static const float32_t sepY;
    static const TexCoords buildingTexCoord[13];
    static const Vector3D buildingVertex[8];
    static const uint32_t rectIndics[6][4];
  protected:
    Vector3D  m_vertex[VertexCount];
    Vector3D  m_normal[VertexCount];
    Texture2D::ptr m_texture;
    ColorRGBA m_color;
    Vector3Dd m_origin;    // ビルの位置
    Vector3D  m_scale;     // ビルのサイズ
    float32_t   m_direction; // ビルの向き(北が0,0-360)
    float32_t   m_lineWidth;
    bool m_drawSolid;
    bool m_drawBorder;
  };

}}

#endif
