// SimpleBuilding3D.hpp
//
// ���z���̃f�[�^
//
//
#ifndef APP_SIMPLEBUILDING3D_HPP
#define APP_SIMPLEBUILDING3D_HPP

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "IBuilding3D.hpp"
#include "BoundaryBoxBuilding.hpp"

namespace app {

  using namespace ts;
  using boost::shared_ptr;
 class SimpleBuilding3D : virtual public IBuilding3D {
    static const size_t IndicsCount = 13; // TriangleStrip�̒��_��
    static const size_t VertexCount = 8;  // ���ۂ̒��_��
  public:
    typedef shared_ptr<SimpleBuilding3D> ptr;
    SimpleBuilding3D(const Vector3Dd& origin = Vector3Dd())
      : m_color(1.0f,1.0f,1.0f,1.0f)
      , m_origin(origin)
      , m_scale(Vector3D(20.0f, 20.0f, 70.0f)) // 20x20x70m�̏����l
      , m_direction(-22.5)
      , m_lineWidth(1.0f)
      , m_drawBorder(true)
      , m_drawSolid(true)
      , m_hitSurface(-1)
      , m_hitVertex(-1)
    {
      calcVertex();
    }

    virtual ~SimpleBuilding3D() {}
    virtual RectF32 getBoundary2D() const { return m_bbRect2D.getRect(); }

    void calcVertex() {
      Matrix matrix;
      matrix.translate(m_origin);
      matrix.rotate(m_direction, Vector3D(0.0f,0.0f,1.0f));
      matrix.scale(m_scale);
      for (size_t n = 0; n < VertexCount; ++n) {
        m_vertex[n] = matrix.mult(ts::gobj::Building3D::buildingVertex[n]);
      }
    }
    void calcVertex64(Vector3Dd* vtx) {
      Matrixd matrix;
      matrix.translate(m_origin);
      matrix.rotate(m_direction, Vector3Dd(0.0f,0.0f,1.0f));
      matrix.scale(m_scale);
      for (size_t n = 0; n < VertexCount; ++n) {
        vtx[n] = matrix.mult(ts::gobj::Building3D::buildingVertex[n]);
      }
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
      if (m_drawSolid) {
        // �ʂ�����
        if (m_texture.get() != 0) {
          // �e�N�X�`������
          m_texture->setColor(ColorRGBA(255,255,255,255));
          m_texture->doRendering(gl);
          beginPrimitive begin(gl, IRenderer::TRIANGLE_STRIP);
          for (size_t n = 0; n < IndicsCount; ++n) {
            gl->texcoord(ts::gobj::Building3D::buildingTexCoord[n]);
            gl->vertex(m_vertex[ts::gobj::Building3D::buildingIndics[n]]);
          }
        }
        else {
          // �e�N�X�`���Ȃ�
          gl->color(m_color);
          gl->disableTexture2D();
          beginPrimitive begin(gl, IRenderer::TRIANGLE_STRIP);
          for (size_t n = 0; n < IndicsCount; ++n) {
            gl->color(m_color);
            gl->vertex(m_vertex[ts::gobj::Building3D::buildingIndics[n]]);
          }
        }
      }
      if (m_drawBorder) {
        // ���C���Ŏl�p�`��`��
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
    virtual bool moveCutSurface(const Vector2D& ) { return false; }

    // �I������Ă��钸�_�̈ړ�
    //
    //           5+----+7
    //          �^|  �^|
    //        3+----+1 |
    //         | 4+-|--+6    z   y
    //         |�^  |�^      | �^  
    //         +----+          ��x
    //        2      0
    // 0:FRB 1:FRT 2:FLB 3:FLT 4:BLB 5:BLT 6:BRB 7:BRT
    virtual bool moveVertex(const Vector2D& move) {
      // �ړ����钸�_�ɑ΂��Ď����I�ɒl��ݒ肷�钸�_�̃e�[�u��
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
      // �܂��A��]�����ɖ߂�
      float32_t dir = this->getDirection();
      // �ړ�����]
      Vector2D mv(move.x, move.y);
      mv.rotate(-dir);
      this->setDirection(0.0f);
      Vector3Dd vtx[8];
      this->calcVertex64(vtx);

      // ���������̈ړ�
      vtx[idx].x += mv.x;
      vtx[idx].y += mv.y;
      for (uint32_t n = 0; n < 8; ++n) {
        if ((tbl[idx][n] & MX) != 0) vtx[n].x = vtx[idx].x;
        if ((tbl[idx][n] & MY) != 0) vtx[n].y = vtx[idx].y;
      }

      Vector2Dd v1(vtx[Building3D::FRB]);
      Vector2Dd v2(vtx[Building3D::BLB]);

      // scale�̍ăZ�b�g
      float64_t sx = fabs((v1.x - v2.x)/2.0);
      float64_t sy = fabs((v1.y - v2.y)/2.0);
      this->setScale(Vector3Dd(sx, sy, this->getScale().z));

      // ���S���W�̍ăZ�b�g
      Vector2Dd origin(this->getOrigin());
      v1.rotate(dir, origin);
      v2.rotate(dir, origin);
      float64_t cx = (v1.x + v2.x) / 2.0;
      float64_t cy = (v1.y + v2.y) / 2.0;
      this->setOrigin(Vector3Dd(cx, cy, vtx[Building3D::FRB].z));

      // ��]�����̍ăZ�b�g
      setDirection(dir);

      // ���W�v�Z
      calcVertex();
      return true;
    }


    // flag control
    virtual void enableSolid(bool enable = true) { m_drawSolid = enable; }
    virtual void enableBorder(bool enable = true) { m_drawBorder = enable; }

    // setter/getter
    virtual void setScale(const Vector3D& scale) { m_scale = scale; calcVertex(); }
    virtual void setDirection(float32_t dir) { m_direction = dir; calcVertex();  }
    virtual void setBbDirection(float32_t dir) { m_direction = dir; calcVertex(); }
    virtual void setOrigin(const Vector3Dd& origin) { m_origin = origin; calcVertex();}
    virtual const Vector3D& getScale() const { return m_scale; }
    virtual float32_t getDirection() const { return m_direction; }
    virtual float32_t getBbDirection() const { return m_direction; }
    virtual const Vector3Dd& getOrigin() { return m_origin; }
    virtual void setLineWidth(float32_t lw) { m_lineWidth = lw; }
    virtual Vector3D& getVertex(size_t n) { return m_vertex[n]; }
    virtual Vector3D* getVertexes() { return &m_vertex[0]; }


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
        // �ǂݍ��ݎ��s
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
      
    virtual void getDrawPrimitives(vector<DrawPrimitive>& /*primitives*/) {
    }
      

    virtual void makeRectangle2D(const Matrix& mat) {
      for (size_t surf = 0; surf < 5; ++surf) {
        gobj::Rectangle3D& rect = m_rect[surf];
        for (size_t pt = 0; pt < 4; ++pt) {
          Vector3D v(getVertex(ts::gobj::Building3D::rectIndics[surf][pt]));
          v = mat.mult2d(v);
          v.z = 999.0f;
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

    // hitCheckVertex2D�Ńq�b�g�������_��Index��Ԃ��B�Ȃ��ꍇ��-1
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

    void getRect2D(RectVector3D& frontRect, RectVector3D& sideRect, RectVector3D& topRect) {
      if (m_rect[FRONT_RECT].isEnable()) {
        copy(frontRect, m_rect[FRONT_RECT].getRectVertex());
      }
      else {
        copy(frontRect, m_rect[BACK_RECT].getRectVertex());
      }
      if (m_rect[RIGHT_RECT].isEnable()) {
        copy(sideRect, m_rect[RIGHT_RECT].getRectVertex());
      }
      else {
        copy(sideRect, m_rect[LEFT_RECT].getRectVertex());
      }
      copy(topRect, m_rect[TOP_RECT].getRectVertex());
    }

    // 2D�摜����e�N�X�`����؂肾��
    virtual bool makeTexture(ImageBufferARGB::ptr mapImg, const RectVector2D& mapRect, MapDirection /*mapDir*/) {
      const uint32_t texSizeX = 512;
      const uint32_t texSizeY = 512;
      const uint32_t texSplitY = 160;
      ImageBufferARGB::ptr image(new ImageBufferARGB(texSizeX,texSizeY));

      int32_t iw = mapImg->getWidth();
      int32_t ih = mapImg->getHeight();
      float32_t mapW = fabs(mapRect[0].x - mapRect[1].x);
      float32_t mapH = fabs(mapRect[2].y - mapRect[1].y);
      float32_t mapL = min(mapRect[0].x, mapRect[1].x);
      float32_t mapT = min(mapRect[2].y, mapRect[1].y);

      RectVector3D fR[3];
      this->getRect2D(fR[0], fR[1], fR[2]);


      // �e�N�X�`�����Q�b�g
      for (uint32_t tex = 0; tex < 3; ++tex) {
        uint32_t tY = texSizeY - (texSizeY * texSplitY / texSizeY);
        uint32_t tX = texSizeX / 2;
        uint32_t sX = tX;
        uint32_t sY = texSizeY * texSplitY / texSizeY;
        if (tex == 1) sX = 0;
        if (tex == 2) { tX = sY; tY = texSizeX/2; sY = 0; sX = 0;} // TOP�̏ꍇ
        try {
          float32_t x1 = fR[tex][0].x;
          float32_t y1 = fR[tex][0].y;
          float32_t x2 = fR[tex][1].x;
          float32_t y2 = fR[tex][3].y;
          float32_t y1L = (fR[tex][1].y - fR[tex][0].y) / tY;
          float32_t x1L = (fR[tex][3].x - fR[tex][0].x) / tX;
          float32_t y2L = (fR[tex][2].y - fR[tex][3].y) / tY;
          float32_t x2L = (fR[tex][2].x - fR[tex][1].x) / tX;

          for (uint32_t y = 0; y < tY; ++y) {
            float32_t fy1 = y1 + y * y1L;
            float32_t fy2 = y2 + y * y2L;
            float32_t fyL = (fy2 - fy1) / tX;
            for (uint32_t x = 0;  x < tX; ++x) {
              float32_t fx1 = x1 + (x * x1L);
              float32_t fx2 = x2 + (x * x2L);
              float32_t fy0 = fy1 + (x * fyL);
              float32_t fx0 = fx1 + (y * (fx2 - fx1) / tY);
              int32_t ix = static_cast<uint32_t>(iw * ((fx0 - mapL) / mapW) + 0.5f);
              int32_t iy = ih - 1 - static_cast<uint32_t>(ih * ((fy0 - mapT) / mapH) + 0.5f);
              int32_t px = sX + (tX - x - 1);
              int32_t py = sY + (tY - y - 1);
              if (tex == 2) {
                py = sX + (tX - x - 1);
                px = sY + y;
              }
              image->setPixelF(px, py, mapImg->getPixel(ix, iy));
            }
          }
        }
        catch (const std::out_of_range&) {
          // �͂ݏo�����H
          return false;
        }
      }
      if (m_texture.get() == 0) {
        m_texture = gobj::Texture2D::ptr(new gobj::Texture2D);
      }
      m_texture->setImageBuffer(image);
      return true;
    }
    virtual void setCutBuildingMode(bool ) {}
    virtual bool doCutBuilding(IBuilding3D::ptr ) {return false; }

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
  protected:
    Vector3D  m_vertex[VertexCount];
    Vector3D  m_normal[VertexCount];
    shared_ptr<Texture2D> m_texture;
    ColorRGBA m_color;
    Vector3Dd m_origin;    // �r���̈ʒu
    Vector3D  m_scale;     // �r���̃T�C�Y
    float32_t   m_direction; // �r���̌���(�k��0,0-360)
    float32_t   m_lineWidth;
    bool m_drawSolid;
    bool m_drawBorder;

  private:
    gobj::Rectangle3D m_rect[6];
    int32_t m_hitSurface;
    int32_t m_hitVertex;

    BoundaryBox<float32_t> m_bbRect2D;


  };
}
#endif
