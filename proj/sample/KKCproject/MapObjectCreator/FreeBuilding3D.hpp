// FreeBuilding3D.hpp
//
// Free建築物のデータ
//
//
#ifndef APP_FREEBUILDING3D_HPP
#define APP_FREEBUILDING3D_HPP

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "IBuilding3D.hpp"
#include "Math/Math.hpp"
#include "TextureAllocator.hpp"
#include "DrawBldgObj.hpp"


namespace app {

  using namespace ts;
  using boost::shared_ptr;


  class FreeBuilding3D : virtual public IBuilding3D {
  public:

   enum VertexPosition {
      FRB = 0, // front right bottom
      FRT, // front right top
      FLB, // front right bottom
      FLT, // ftont left top
      BLB,
      BLT,
      BRB,
      BRT,
      VertexPositionCount
    };

    enum RectSurface {
      FRONT_RECT = 0,
      LEFT_RECT,
      BACK_RECT,
      RIGHT_RECT,
      TOP_RECT
    };

    enum PrimitiveType {
      Nothing,
      Cube,       // 直方体
      Cylinder,   // 円柱
      RectTorus,  // トーラス状の四角形
    };

    typedef shared_ptr<FreeBuilding3D> ptr;
    FreeBuilding3D(const Vector3Dd& origin = Vector3Dd())
      : m_color(255,200,200,150)
      , m_origin(origin)
      , m_scale(Vector3D(1.0f, 1.0f, 1.0f)) // 建物を含むバウンダリボックスのサイズ。単位はメートル。
      , m_direction(0.0f)
      , m_bbDirection(0.0f)
      , m_lineWidth(1.0f)
      , m_drawBorder(true)
      , m_drawSolid(true)
      , m_hitSurface(-1)
      , m_hitVertex(-1)
      , m_calcVertex(false)
      , m_cutSurfacePos(0.5f)
      , m_cutBuldingMode(false)
    {
    }
    FreeBuilding3D(PrimitiveType ptp, const Vector3D& size, const Vector3Dd& origin = Vector3Dd())
      : m_color(255,200,200,150)
      , m_origin(origin)
      , m_scale(Vector3D(1.0f, 1.0f, 1.0f)) // 建物を含むバウンダリボックスのサイズ。単位はメートル。
      , m_direction(0.0f)
      , m_bbDirection(0.0f)
      , m_lineWidth(1.0f)
      , m_drawBorder(true)
      , m_drawSolid(true)
      , m_hitSurface(-1)
      , m_hitVertex(-1)
      , m_calcVertex(false)
      , m_cutSurfacePos(0.5f)
      , m_cutBuldingMode(false)
    {
      createPrimitive(ptp, size);
    }

    virtual ~FreeBuilding3D() {}

    void addDrawObj(DrawBldgObj& dobj) {
      m_drawObj.push_back(dobj);
    }

    void addQuads(vector<Vector3D>& vertex) {
      m_drawObj.push_back(DrawBldgObj(IRenderer::QUADS, vertex));
    }
    void addTriangles(vector<Vector3D>& vertex) {
      m_drawObj.push_back(DrawBldgObj(IRenderer::TRIANGLES, vertex));
    }
    void addTriangleStrip(vector<Vector3D>& vertex) {
      m_drawObj.push_back(DrawBldgObj(IRenderer::TRIANGLE_STRIP, vertex));
    }

    void clear() {
      m_drawObj.clear();
      m_calcVertex = false;

    }

    // モデルビューマトリクスを取得する
    // bbBox == trueの場合はバウンダリボックスのマトリクス
    Matrix getModelMatrix(bool bbBox = false) const {
      Matrix mm;
      mm.translate(m_origin);
      mm.rotate(bbBox ? m_direction + m_bbDirection : m_direction, Vector3D(0.0f,0.0f,1.0f));
      mm.scale(m_scale);
      return mm;
    }

    // 頂点データの正規化
    // addXXXで頂点データを登録した後にvertexNormalizationで正規化する
    void vertexNormalization() {

      // バウンダリボックスの計算
      BoundaryBox<float32_t> bb;
      // まず中心を求める
      BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
        BOOST_FOREACH(Vector3D& v, dobj.vertex) {
          bb.set(v);
        }
      }
      Vector3D center = bb.center();
      center.z = static_cast<float32_t>(m_origin.z);
      //DEBUGLOG("center=%f,%f\n", center.x, center.y);

      // 中心からバウンダリボックスの回転分を戻す
      Matrix bbMat;
      bbMat.rotate(-m_bbDirection, Vector3D(0.0f,0.0f,1.0f));
      bbMat.translate(-center);
      bb.clear();
      BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
        BOOST_FOREACH(Vector3D& v, dobj.vertex) {
          bb.set(bbMat.mult(v));
        }
      }

      if (!center.isZero()) {
        m_origin += center; // オブジェクトの中心をoriginに設定する
        // 中心位置に座標を移動
        BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
          BOOST_FOREACH(Vector3D& v, dobj.vertex) {
            v -= center;
          }
        }
      }

      // バウンダリボックスの頂点を生成
      m_bbVertex.resize(8);
      m_bbVertex[FRB] = Vector3D(bb.max.x, bb.min.y, bb.min.z); // front right bottom
      m_bbVertex[FRT] = Vector3D(bb.max.x, bb.min.y, bb.max.z); // front right top
      m_bbVertex[FLB] = Vector3D(bb.min.x, bb.min.y, bb.min.z); // front left  bottom
      m_bbVertex[FLT] = Vector3D(bb.min.x, bb.min.y, bb.max.z); // front left  top
      m_bbVertex[BLB] = Vector3D(bb.min.x, bb.max.y, bb.min.z); // back  left  bottom
      m_bbVertex[BLT] = Vector3D(bb.min.x, bb.max.y, bb.max.z); // back  left  top
      m_bbVertex[BRB] = Vector3D(bb.max.x, bb.max.y, bb.min.z); // back  right bottom
      m_bbVertex[BRT] = Vector3D(bb.max.x, bb.max.y, bb.max.z); // back  right top

      // 切断面の計算
      setCutSurface();
    }

    // 切断面の計算
    void setCutSurface() {

      m_cutSurface[0] = (m_bbVertex[FRB] + m_bbVertex[BRB]) / 2.0f;
      m_cutSurface[1] = (m_bbVertex[FRT] + m_bbVertex[BRT]) / 2.0f;
      m_cutSurface[2] = (m_bbVertex[FLT] + m_bbVertex[BLT]) / 2.0f;
      m_cutSurface[3] = (m_bbVertex[FLB] + m_bbVertex[BLB]) / 2.0f;
      float32_t y = (m_bbVertex[FRB].y * m_cutSurfacePos) + (m_bbVertex[BRB].y * (1.0f - m_cutSurfacePos));

      for (uint32_t n = 0; n < 4; ++n) {
        m_cutSurface[n].x *= 1.5f;
        m_cutSurface[n].y = y;
        m_cutSurface[n].z *= 1.5f;
      }

    }

    // オブジェクトの切断
    virtual bool doCutBuilding(IBuilding3D::ptr newbldg) {
      FreeBuilding3D::ptr clone(dynamic_pointer_cast<FreeBuilding3D>(newbldg));
      if (isNull(clone)) { return false; }
      clone->m_scale = m_scale;
      clone->m_origin = m_origin;
      clone->m_direction = m_direction;

      Matrix mm1;
      mm1.rotate(-m_bbDirection, Vector3D(0.0f,0.0f,1.0f));
      Matrix mm2;
      mm2.rotate(m_bbDirection, Vector3D(0.0f,0.0f,1.0f));

      Vertex3DSet crossVertex; // 断面の頂点集合
      vector<DrawBldgObj> myObj;
      BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
        vector<DrawBldgObj> o1;
        vector<DrawBldgObj> o2;
        mm1.multVector(dobj.vertex);
        dobj.splitSurface(m_cutSurface[0], m_cutSurface[1], m_cutSurface[2],  o1, o2, crossVertex);
        myObj.insert(myObj.end(), o1.begin(), o1.end());
        clone->m_drawObj.insert(clone->m_drawObj.end(), o2.begin(), o2.end());
      }
      m_drawObj = myObj;
      // 断面の頂点リストを左まわりにソートする
      crossVertex.aroundSort((m_cutSurface[1]-m_cutSurface[0]).cross(m_cutSurface[2]-m_cutSurface[0])); // 反時計回りにソート
      // 断面の描画オブジェクトの生成
      DrawBldgObj crossObj(IRenderer::TRIANGLE_FAN);
      crossVertex.makeTriangleFan(crossObj.vertex); // TriangleFanのリストを作る
      clone->m_drawObj.push_back(crossObj);
      // 分離側はオブジェクトの向きを反転する
      crossObj.vertex.clear();
      crossVertex.makeTriangleFan(crossObj.vertex, true);  // 回転を逆にする

      m_drawObj.push_back(crossObj);

      BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
        mm2.multVector(dobj.vertex);
      }
      BOOST_FOREACH(DrawBldgObj& dobj, clone->m_drawObj) {
        mm2.multVector(dobj.vertex);
      }

      vertexNormalization();
      clone->vertexNormalization();
      return true;
    }


    void calcVertex64(Vector3D* vtx) {
      Matrix mm = getModelMatrix(true);
      for (uint32_t n = 0; n < VertexPositionCount; ++n) {
        vtx[n] = mm.mult(getBbVertex(static_cast<VertexPosition>(n)));
      }
    }
    virtual bool moveCutSurface(const Vector2D& move) {
      // 建物分割モードの時は分割位置の調整
      float32_t v = (move.x + move.y) / 100.0f;
      m_cutSurfacePos -= v;
      if (m_cutSurfacePos < 0.0f) m_cutSurfacePos = 0.0f;
      if (m_cutSurfacePos > 1.0f) m_cutSurfacePos = 1.0f;
      setCutSurface();
      return true;
    }


    virtual bool moveVertex(const Vector2D& move)  {
      // 移動する頂点に対して自動的に値を設定する頂点のテーブル
      enum MvXy { NC = 0, MX = 1, MY = 2, XY = 3};
      static const MvXy tbl[8][8] = { 
        // FRB FRT FLB FLT BLB BLT BRB BRT
         { NC, XY, MY, MY, NC, NC, MX, MX }, // FRB 
         { XY, NC, MY, MY, NC, NC, MX, MX }, // FRT 
         { MY, MY, NC, XY, MX, MX, NC, NC }, // FLB
         { MY, MY, XY, NC, MX, MX, NC, NC }, // FLT
         { NC, NC, MX, MX, NC, XY, MY, MY }, // BLB
         { NC, NC, MX, MX, XY, NC, MY, MY }, // BLT
         { MX, MX, NC, NC, MY, MY, NC, XY }, // BRB
         { MX, MX, NC, NC, MY, MY, XY, NC }  // BRT
      };

      int32_t idx = this->getHitVertexIndex();
      if (idx < 0) return false;
      // まず、回転を元に戻す
      float32_t dir = m_direction;
      float32_t bbdir = m_bbDirection;
      // 移動も回転
      Vector2D mv(move);
      mv.rotate(-(m_direction + m_bbDirection));
      m_direction = 0.0f;
      m_bbDirection = 0.0f;

      Vector3D vtx[8];
      this->calcVertex64(vtx);

      // 水平方向の移動
      vtx[idx].x += mv.x;
      vtx[idx].y += mv.y;
      for (uint32_t n = 0; n < 8; ++n) {
        if ((tbl[idx][n] & MX) != 0) vtx[n].x = vtx[idx].x;
        if ((tbl[idx][n] & MY) != 0) vtx[n].y = vtx[idx].y;
      }

      Matrix mm;
      mm.scale(1.0f / m_scale);
      mm.translate(-m_origin);
      for (uint32_t n = 0; n < 8; ++n) {
        vtx[n] = mm.mult(vtx[n]);
      }

      Vector2D ov1(m_bbVertex[FRB]);
      Vector2D ov2(m_bbVertex[BLB]);
      Vector2D nv1(vtx[FRB]);
      Vector2D nv2(vtx[BLB]);

      // BBboxのscale
      Vector3D bbscale(fabs(nv1.x - nv2.x)/fabs(ov1.x - ov2.x),
                       fabs(nv1.y - nv2.y)/fabs(ov1.y - ov2.y), 1.0f);

      // 中心座標の再セット
      Vector3D offset((nv1.x + nv2.x) / 2.0f, (nv1.y + nv2.y) / 2.0f, 0.0f);
      nv1.rotate(dir+bbdir);
      nv2.rotate(dir+bbdir);
      Vector2D origOffset((nv1.x + nv2.x) / 2.0f, (nv1.y + nv2.y) / 2.0f);
      m_origin.x += origOffset.x;
      m_origin.y += origOffset.y;

      for (uint32_t n = 0; n < 8; ++n) {
        m_bbVertex[n] = vtx[n] - offset;
      }

      // 各頂点をbbBoxのscale分だけ拡大・縮小する
      Matrix bbMat;
      bbMat.rotate(bbdir, Vector3D(0.0f,0.0f,1.0f));
      bbMat.scale(bbscale);
      bbMat.rotate(-bbdir, Vector3D(0.0f,0.0f,1.0f));
      BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
        bbMat.multVector(dobj.vertex);
      }

      // 回転方向の再セット
      m_direction = dir;
      m_bbDirection = bbdir;

      // 座標計算
      //vertexNormalization();

      // 切断面の計算
      setCutSurface();
      return true;
    }


    virtual void setTexture2D(shared_ptr<Texture2D> texture) {
      m_texture = texture;
      m_drawSolid = true;
      m_drawBorder = false;
      if (texture.get() != 0) {
        m_texture->setColor(m_color);
      }
    }

    virtual Texture2D::ptr& getTexture2D() {
      return m_texture;
    }


    virtual void setColor(const ColorRGBA& col) { 
      m_color = col; 
      if (m_texture.get() != 0) {
         m_texture->setColor(m_color);
      }
    }
 
    virtual void doRendering(IRenderer* gl) {
      gl->pushMatrix();
      gl->translation(m_origin);
      gl->rotate(m_direction, Vector3D(0.0f,0.0f,1.0f));
      gl->scale(m_scale);
      ::glEnable(GL_BLEND);
      if (m_drawSolid) {
        // 面を書く
        if (m_texture.get() != 0) {
          // テクスチャあり
          ::glDisable(GL_BLEND);
          gl->color(ColorRGBA(255,255,255,255));
          m_texture->doRendering(gl);
          gl->color(ColorRGBA(255,255,255,255));
          for (size_t n = 0; n < m_drawObj.size(); ++n) {
            m_drawObj[n].doRendering(gl);
          }
        }
        else {
          // テクスチャなし
          gl->color(m_color);
          gl->disableTexture2D();
          for (size_t n = 0; n < m_drawObj.size(); ++n) {
          gl->color(ColorRGBA(m_color.r, m_color.g, m_color.b, 255));
            m_drawObj[n].doRenderingWireFrame(gl);
            gl->color(m_color);
            m_drawObj[n].doRendering(gl);
            
          }
        }
      }
      if (m_drawBorder && !m_drawSolid) {
        // ラインで四角形を描画
          ::glEnable(GL_BLEND);
          gl->disableTexture2D();
          gl->lineWidth(1.0f);
          for (size_t n = 0; n < m_drawObj.size(); ++n) {
              gl->color(ColorRGBA(255,100,100,255));
              m_drawObj[n].doRenderingWireFrame(gl);
              gl->color(ColorRGBA(255,200,200,100));
              m_drawObj[n].doRendering(gl);
          }
      }
      gl->popMatrix();
      if (m_drawBorder) {

        gl->pushMatrix();
        gl->translation(m_origin);
        gl->rotate(m_direction + m_bbDirection, Vector3D(0.0f,0.0f,1.0f));
        gl->scale(m_scale);

        // 切断面
        ::glEnable(GL_BLEND);
        if (m_cutBuldingMode) {
          ::glDisable(GL_CULL_FACE);
          gl->drawSolidRectangle(m_cutSurface, ColorRGBA(0,0,255,128));
          ::glEnable(GL_CULL_FACE);
        }

        gl->color(ColorRGBA(180,180,180,255));
        gl->disableTexture2D();
        gl->lineWidth(m_lineWidth);
        gl->begin(IRenderer::LINE_LOOP);
        gl->vertex(m_bbVertex[1]);
        gl->vertex(m_bbVertex[3]);
        gl->vertex(m_bbVertex[5]);
        gl->vertex(m_bbVertex[7]);
        gl->end();
        gl->begin(IRenderer::LINE_LOOP);
        gl->vertex(m_bbVertex[0]);
        gl->vertex(m_bbVertex[2]);
        gl->vertex(m_bbVertex[4]);
        gl->vertex(m_bbVertex[6]);
        gl->end();
        gl->begin(IRenderer::LINE);
        gl->vertex(m_bbVertex[0]);
        gl->vertex(m_bbVertex[1]);
        gl->vertex(m_bbVertex[2]);
        gl->vertex(m_bbVertex[3]);
        gl->vertex(m_bbVertex[4]);
        gl->vertex(m_bbVertex[5]);
        gl->vertex(m_bbVertex[6]);
        gl->vertex(m_bbVertex[7]);
        gl->end();


        gl->popMatrix();

      }
    }

    // flag control
    virtual void enableSolid(bool enable = true) { m_drawSolid = enable; }
    virtual void enableBorder(bool enable = true) { m_drawBorder = enable; }

    // setter/getter
    virtual void setScale(const Vector3D& scale) { m_scale = scale; }
    virtual void setDirection(float32_t dir) { m_direction = dir; }
    virtual void setBbDirection(float32_t dir) { m_bbDirection = dir; vertexNormalization(); }
    virtual void setOrigin(const Vector3Dd& origin) { m_origin = origin; }
    virtual const Vector3D& getScale() const { return m_scale; }
    virtual float32_t getDirection() const { return m_direction; }
    virtual float32_t getBbDirection() const { return m_bbDirection; }
    virtual const Vector3Dd& getOrigin() { return m_origin; }
    virtual void setLineWidth(float32_t lw) { m_lineWidth = lw; }

    Vector3D getBbVertex(VertexPosition n) {
      return m_bbVertex[n];
    }




    virtual RectF32 getBoundary2D() const { return m_bbRect2D.getRect(); }

    virtual void setRectColor(const ColorRGBA& col) {
      for (size_t suf = 0; suf < 5; ++suf) {
        m_rect[suf].setColor(col);
      }
    }

    virtual bool hitCheckVertex2D(const Vector2D& v, const Vector2D& range) {
      for (uint32_t suf = 0; suf < 5; ++suf) {
        if (m_rect[suf].isEnable()) {
          for (uint32_t vertex = 0; vertex < 4; ++vertex) {
            if (v.distance(Vector2D(m_rect[suf].getVertex(vertex))) <= range.y * 8.0f) {
              m_hitSurface = suf;
              m_hitVertex = vertex;
              return true;
            }
          }
        }
      }
      m_hitSurface = -1;
      m_hitVertex = -1;
      return false;
    }

    // hitCheckVertex2Dでヒットした頂点のIndexを返す。ない場合は-1
    virtual int32_t getHitVertexIndex() const {
      if (m_hitSurface == -1 || m_hitVertex == -1) return -1;
      return ts::gobj::Building3D::rectIndics[m_hitSurface][m_hitVertex];
    }

    virtual bool isPointInRect2D(const Vector2D& xy) {
      for (size_t suf = 0; suf < 5; ++suf) {
        gobj::Rectangle3D& rect(m_rect[suf]);
        if (SideCheck2D(rect.getVertex(0), rect.getVertex(1), xy) >= 0) continue;
        if (SideCheck2D(rect.getVertex(1), rect.getVertex(2), xy) >= 0) continue;
        if (SideCheck2D(rect.getVertex(2), rect.getVertex(3), xy) >= 0) continue;
        if (SideCheck2D(rect.getVertex(3), rect.getVertex(0), xy) >= 0) continue;
        return true;
      }
      return false;
    }
    virtual void doRenderingRect(IRenderer* gl, const Vector2D& pixSize) {
      for (int32_t r = 0; r < 5; ++r) {
        if (m_rect[r].isEnable()) {
          m_rect[r].doRendering(gl);
          if (r == m_hitSurface && m_hitVertex != -1) {
            Vector3D v(m_rect[r].getVertex(m_hitVertex));
            gl->enableDepthTest(false);
            gl->disableTexture2D();
            gl->lineWidth(1.0f);
            gl->color(ColorRGBA(0, 255, 0, 255));
            gl->begin(IRenderer::LINE_LOOP);
            gl->vertex(Vector3D(v.x-pixSize.x*4.0f, v.y-pixSize.y*4.0f, v.z));
            gl->vertex(Vector3D(v.x+pixSize.x*4.0f, v.y-pixSize.y*4.0f, v.z));
            gl->vertex(Vector3D(v.x+pixSize.x*4.0f, v.y+pixSize.y*4.0f, v.z));
            gl->vertex(Vector3D(v.x-pixSize.x*4.0f, v.y+pixSize.y*4.0f, v.z));
            gl->end();
            gl->enableDepthTest();
          }
        }
      }
    }


    // バウンダリボックス5面の2D系の座標を計算する
    // 2D系のバウンダリボックスも作る
    virtual void makeRectangle2D(const Matrix& mat) {
      Matrix mm = getModelMatrix(true);

      m_projectionMatrix = mat;
      m_bbRect2D.clear();
      for (size_t surf = 0; surf < 5; ++surf) {
        gobj::Rectangle3D& rect = m_rect[surf];
        for (size_t pt = 0; pt < 4; ++pt) {
          Vector3D v(getBbVertex(static_cast<VertexPosition>(ts::gobj::Building3D::rectIndics[surf][pt])));
          v = mm.mult(v);
          v = mat.mult2d(v);
          v.z = 999.0f;
          m_bbRect2D.set(v);
          rect.setVertex(pt, v);
        }
        Vector2D v2a(rect.getVertex(1) - rect.getVertex(0));
        Vector2D v2b(rect.getVertex(2) - rect.getVertex(1));
        if (v2a.cross(v2b) < 0.0f) {
          rect.disable();
        }
        else {
          rect.enable();
        }
        rect.setLineWidth(3.0f);
        rect.setColor(ColorRGBA(255,255,0,255));
      }
    }

#if 0
    //
    //  テクスチャのイメージは東西南北４分割で保存される
    //
    //  +--------+--------+
    //  |   北   |   南   |
    //  +--------+--------+
    //  |   西   |   東   |
    //  +--------+--------+
    //
    //
    const RectF32& getTextureRect(MapDirection mapDir) const {
      static const RectF32 n(0.0f, 0.0f, 0.5f, 0.5f);
      static const RectF32 s(0.5f, 0.0f, 0.5f, 0.5f);
      static const RectF32 e(0.0f, 0.5f, 0.5f, 0.5f);
      static const RectF32 w(0.5f, 0.5f, 0.5f, 0.5f);
      static const RectF32 norect;
      switch (mapDir) {
      case MAPDIR_NORTH: return n;
      case MAPDIR_SOUTH: return s;
      case MAPDIR_EAST:  return e;
      case MAPDIR_WEST:  return w;
      default:
        return norect;
      }
    }
#endif

    virtual bool makeTexture(ImageBufferARGB::ptr mapImg, const RectVector2D& mapRect, MapDirection mapDir) {
      uint32_t texSizeX = 256;
      uint32_t texSizeY = 256;
      MeasureTime time;
      time.start();

      try {

        int32_t iw = mapImg->getWidth();
        int32_t ih = mapImg->getHeight();
        float32_t mapW = (fabs(mapRect[0].x - mapRect[1].x) + fabs(mapRect[2].x - mapRect[3].x)) / 2.0f;
        float32_t mapH = (fabs(mapRect[2].y - mapRect[1].y) + fabs(mapRect[3].y - mapRect[0].y)) / 2.0f;
        float32_t mapL = (mapRect[1].x + mapRect[2].x) / 2.0f;
        float32_t mapT = (mapRect[2].y + mapRect[3].y) / 2.0f;

        // 写真画像をテクスチャにコピーする

        RectF32 texRect(m_bbRect2D.getRect()); // 2D画面上のテクスチャ領域
        RectS32 imgRect;
        imgRect.left = static_cast<int32_t>(iw * (texRect.left - mapL) / mapW + 0.5f);
        imgRect.width = static_cast<int32_t>(iw * texRect.width / mapW + 0.5f);
        imgRect.top = static_cast<int32_t>(ih * (texRect.top - mapT) / mapH + 0.5f);
        imgRect.height = static_cast<int32_t>(ih * texRect.height / mapH + 0.5f);


#if 1
        // 写真毎にイメージを作る
        texSizeX = imgRect.width;
        texSizeY = imgRect.height;
        ImageBufferARGB::ptr image(new ImageBufferARGB(texSizeX,texSizeY));
        TRACE("TextureSize:%d,%d\n", texSizeX, texSizeY);
        imgRect.top = ih - imgRect.top - imgRect.height;
       // imgRect.top -= 16;
        //imgRect.left += 8;
        image->bitblt(0,0, mapImg.get(), imgRect);
        // バウンダリボックスをテクスチャ領域として、テクスチャ座標を設定する
        Matrix matrix;
        matrix = m_projectionMatrix;
        matrix.translate(m_origin);
        matrix.rotate(m_direction, Vector3D(0.0f,0.0f,1.0f));
        matrix.scale(m_scale);

        // テクスチャアロケータに追加
        m_texAllocator.addImage(mapDir, image, matrix, m_bbRect2D.getRect());

        BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
          dobj.setTextureCoord(mapDir, m_texAllocator);
        }
        if (m_texture.get() == 0){
          m_texture = gobj::Texture2D::ptr(new gobj::Texture2D);
        }
        m_texture->setImageBuffer(m_texAllocator.makeTextureImage());
        m_drawSolid = true;
        time.stop();
        mstring text;
        text.format("0:make texture time:%.3lfms", time.getTotalTime() * 1000.0);
       // AppStatusManager::SetStatusText(text);

        return true;
#else
        //  テクスチャのイメージは東西南北４分割で保存される
        //
        //  +--------+--------+
        //  |   北   |   南   |
        //  +--------+--------+
        //  |   西   |   東   |
        //  +--------+--------+

        ImageBuffer::ptr image;
        if (m_texture.get() != 0 && m_texture->getImageBuffer().get() != 0) {
          // imageがすでにあるのでそれを使う
          image = m_texture->getImageBuffer();
          texSizeX = image->getWidth() / 2;
          texSizeY = image->getHeight() / 2;
        }
        else {
          uint32_t texSize = max(imgRect.width, imgRect.height);
          texSize = 1 << intlog2(texSize);
          texSizeX = texSize;
          texSizeY = texSize;
          image = ImageBufferARGB::ptr(new ImageBufferARGB(texSizeX*2,texSizeY*2));
          image->clear(ColorRGBA(255,255,255,255));
        }
        const RectF32& texOffset = getTextureRect(mapDir);
        uint32_t imgXofs = static_cast<uint32_t>(texOffset.left * image->getWidth() + 0.5f);
        uint32_t imgYofs = static_cast<uint32_t>(texOffset.top * image->getHeight() + 0.5f);
        
        TRACE("TextureSize:%d,%d\n", texSizeX, texSizeY);
        // テクスチャをリサイズしながらコピー
        for (uint32_t y = 0; y < texSizeY; ++y) {
            float fy0 = texRect.top +  (texRect.height * y / texSizeY);
            int32_t iy = static_cast<uint32_t>(ih * ((fy0 - mapT) / mapH) + 0.5f);
            iy = ih - iy - 1;
          for (uint32_t x = 0; x < texSizeX; ++x) {
            float fx0 = texRect.left + (texRect.width * x / texSizeX);
            int32_t ix = static_cast<uint32_t>(iw * ((fx0 - mapL) / mapW) + 0.5f);
            image->setPixelF(imgXofs + x, imgYofs + texSizeY - y - 1, mapImg->getPixel(ix, iy));
          }
        }
        // バウンダリボックスをテクスチャ領域として、テクスチャ座標を設定する
        Matrix matrix;
        matrix = m_projectionMatrix;
        matrix.translate(m_origin);
        matrix.scale(m_scale);

        BOOST_FOREACH(DrawBldgObj& dobj, m_drawObj) {
          dobj.setTextureCoord(mapDir, matrix, m_bbRect2D, texOffset);
        }
        if (m_texture.get() == 0){
          m_texture = gobj::Texture2D::ptr(new gobj::Texture2D);
        }
        m_texture->setImageBuffer(image);
        m_drawSolid = true;
        time.stop();
        mstring text;
        text.format("0:make texture time:%.3lfms", time.getTotalTime() * 1000.0);
       // AppStatusManager::SetStatusText(text);

        return true;
#endif
      }
      catch (const std::out_of_range&) {
        // はみ出した？
        time.stop();
        return false;
      }

    }

    virtual void getDrawPrimitives(vector<DrawPrimitive>& primitives) {
      Matrix matrix;
//      matrix.translate(m_origin);
      matrix.scale(m_scale);


      BOOST_FOREACH(DrawBldgObj& obj, m_drawObj) {
        DrawPrimitive data = static_cast<DrawPrimitive>(obj);
        BOOST_FOREACH(Vector3D& v, data.vertex) {
          v = matrix.mult(v);
        }
        primitives.push_back(data);
      }
    }

    virtual void serialize(bstream& bs) {
      bstream tmp;
      tmp << m_drawObj;
      tmp << m_color;
      tmp << m_origin;
      tmp << m_scale;
      tmp << m_direction;
      tmp << m_bbDirection;
      tmp << m_lineWidth;
      tmp << m_drawSolid;
      tmp << m_drawBorder;
      bs << tmp;
    }

    virtual void deserialize(const bstream& bs) {
      bstream tmp;
      bs >> tmp;
      tmp >> m_drawObj;
      tmp >> m_color;
      tmp >> m_origin;
      tmp >> m_scale;
      tmp >> m_direction;
      tmp >> m_bbDirection;
      tmp >> m_lineWidth;
      tmp >> m_drawSolid;
      tmp >> m_drawBorder;

      vertexNormalization();
    }


    virtual void setCutBuildingMode(bool enable) { 
      m_cutBuldingMode = enable;
    }

    // プリミティブを生成する
    void createPrimitive(PrimitiveType ptp, const Vector3D& size) {
      Vector3D sz(fabs(size.x/2.0f), fabs(size.y/2.0f), fabs(size.z/2.0f));
      switch (ptp) {
      case Cube: // 直方体
        {
          static const Vector3Di cube[] = {
            Vector3Di(-1, -1,  0), Vector3Di( 1, -1, 0), Vector3Di( 1, -1, 1), Vector3Di(-1, -1, 1),
            Vector3Di( 1, -1,  0), Vector3Di( 1,  1, 0), Vector3Di( 1,  1, 1), Vector3Di( 1, -1, 1),
            Vector3Di( 1,  1,  0), Vector3Di(-1,  1, 0), Vector3Di(-1,  1, 1), Vector3Di( 1,  1, 1),
            Vector3Di(-1,  1,  0), Vector3Di(-1, -1, 0), Vector3Di(-1, -1, 1), Vector3Di(-1,  1, 1),
            Vector3Di(-1, -1,  1), Vector3Di( 1, -1, 1), Vector3Di( 1,  1, 1), Vector3Di(-1,  1, 1) 
          };
          vector<Vector3D> vtx;
          for (uint32_t n = 0; n < 4*5; ++n) {
            vtx.push_back(sz * cube[n]);
          }
          addQuads(vtx);
          vertexNormalization();
        }
        break;
      default:
        break;
      }
    }

  protected:
    vector<DrawBldgObj> m_drawObj;
    vector<Vector3D>  m_bbVertex;
    shared_ptr<Texture2D> m_texture;
    ColorRGBA m_color;
    Vector3Dd m_origin;    // ビルの位置
    Vector3D  m_scale;     // ビルのサイズ
    float32_t m_direction; // ビルの向き(北が0,0-360)
    float32_t m_bbDirection; // m_directionに対するバウンダリボックスの向き
    float32_t m_lineWidth;
    bool m_drawSolid;
    bool m_drawBorder;

  private:
    gobj::Rectangle3D m_rect[6];
    int32_t m_hitSurface;
    int32_t m_hitVertex;
    bool m_calcVertex;
    BoundaryBox<float32_t> m_bbRect2D;
    Matrix m_projectionMatrix;

    TextureAllocator m_texAllocator;
    RectVector3D m_cutSurface;
    float m_cutSurfacePos;
    bool m_cutBuldingMode;

  };

}
#endif
