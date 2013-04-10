// IBuilding3D.hpp
//
// 建築物のデータ
//
//
#ifndef APP_IBUILDING3D_HPP
#define APP_IBUILDING3D_HPP

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "Graphics/Building3D.h"
#include "TextureManager.hpp"

namespace app {

  using namespace ts;
  using boost::shared_ptr;

  
  // 描画オブジェクトクラス
  struct DrawPrimitive {
    DrawPrimitive() {}
    DrawPrimitive(IRenderer::PrimitiveType type_)
      : type(type_)
    {}
    DrawPrimitive(IRenderer::PrimitiveType type_, vector<Vector3D>& vertex_)
      : type(type_), vertex(vertex_)
    {}
    virtual ~DrawPrimitive() {}

    IRenderer::PrimitiveType type;
    vector<Vector3D> vertex;
    vector<TexCoords> texCoord;
    vector<Vector3D> normal;  // 法線ベクトル
  };



  class IBuilding3D : public gobj::Node {
  public:
    typedef shared_ptr<IBuilding3D> ptr;
    virtual ~IBuilding3D() {}

    virtual void setColor(const ColorRGBA& col) = 0;
    virtual void enableSolid(bool) = 0;
    virtual void enableBorder(bool) = 0;
    virtual void setTexture2D(gobj::Texture2D::ptr) = 0;
    virtual void setOrigin(const Vector3Dd& origin) = 0;
    virtual const Vector3Dd& getOrigin() = 0;
    virtual void setScale(const Vector3D& scale) = 0;
    virtual const Vector3D& getScale() const = 0;
    virtual float32_t getDirection() const = 0;
    virtual float32_t getBbDirection() const = 0;
    virtual void setDirection(float32_t dir) = 0;
    virtual void setBbDirection(float32_t dir) = 0;
    virtual gobj::Texture2D::ptr& getTexture2D() = 0;
    virtual void setLineWidth(float32_t lw) = 0;
    virtual bool moveVertex(const Vector2D& move)  = 0;
    virtual bool moveCutSurface(const Vector2D& move)  = 0;

    virtual void makeRectangle2D(const Matrix& mat) = 0;
    virtual void setRectColor(const ColorRGBA& col) = 0;
    virtual bool hitCheckVertex2D(const Vector2D& v, const Vector2D& range) = 0;
    virtual bool makeTexture(ImageBufferARGB::ptr mapImg, const RectVector2D& mapRect, MapDirection mapDir) = 0;
    virtual RectF32 getBoundary2D() const = 0;

    // hitCheckVertex2Dでヒットした頂点のIndexを返す。ない場合は-1
    virtual int32_t getHitVertexIndex() const = 0;
    virtual bool isPointInRect2D(const Vector2D& xy)  = 0;
    virtual void doRenderingRect(IRenderer* gl, const Vector2D& pixSize) = 0;
    virtual void getDrawPrimitives(vector<DrawPrimitive>& primitives) = 0;
    virtual void setCutBuildingMode(bool enable) = 0;

    virtual bool doCutBuilding(IBuilding3D::ptr newbldg) = 0;


  };





}


// bstream用のオペレーター定義
namespace ts {
  template <> inline    const bstream& operator >> (const bstream& bs, DrawPrimitive& s) {
    bs >> s.type;
    bs >> s.vertex;
    bs >> s.texCoord;
    bs >> s.normal;
    return bs; 
  }
  template <> inline    bstream& operator << (bstream& bs, const DrawPrimitive& s) {
    bs << static_cast<uint32_t>(s.type);
    bs << s.vertex;
    bs << s.texCoord;
    bs << s.normal;
    return bs; 
  }
}


#endif
