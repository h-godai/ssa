// BuildingObject
//
// 地上にある建築物のデータ
//
//
#ifndef BUILDINGOBJECT_HPP
#define BUILDINGOBJECT_HPP

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "Graphics/GraphicsObject.h"
#include "Graphics/Building3D.h"
#include "PlatformDependent/Timer/MeasureTime.hpp"
#include "Util/ustring.hpp"

#include "TextureManager.hpp"
#include "AeroPhotoObject.hpp"
#include "IBuilding3D.hpp"
#include "SimpleBuilding3D.hpp"
#include "FreeBuilding3D.hpp"

namespace app {

  using namespace ts;
  using boost::shared_ptr;

  const uint32_t BuldingObjectVersion = 1;

  class BuildingObject {
  public:
    typedef shared_ptr<BuildingObject> ptr;
    enum BuildingType {
      SIMPLE_BUILDING=0,
      FREE_BUILDING,
      BUILDING_TYPE3,
      NOTDEFINED
    };
    enum Status {
      UNSELECTED,
      SELECTED,
      FOCUSED
    };
    enum TextureSurface {
      TOP_TEXTURE,
      FRONT_TEXTURE,
      SIDE_TEXTURE,
      BUMP_TEXTURE,
      ENV_TEXTURE,
      NO_TEXTURE
    };

    BuildingObject(BuildingType type=NOTDEFINED, const Vector3Dd& origin = Vector3Dd())
      : m_version(BuldingObjectVersion)
      , m_objectId(++m_lastId)
      , m_parentObjectId(0)
      , m_type(type)
      , m_modified(false)
      , m_status(UNSELECTED)
      , m_currentTextureSurface(FRONT_TEXTURE)
    {
      switch(type) {
      case SIMPLE_BUILDING:
        m_building3D = IBuilding3D::ptr(new SimpleBuilding3D(origin));
        break;
      case FREE_BUILDING:
        m_building3D = IBuilding3D::ptr(new FreeBuilding3D(origin));
        break;
      }
      m_textureName.format("tex%7.7d.png", m_objectId);
    }

    BuildingObject(BuildingType type, IBuilding3D::ptr bld3D, uint32_t parent = 0)
      : m_version(BuldingObjectVersion)
      , m_objectId(++m_lastId)
      , m_parentObjectId(parent)
      , m_type(type)
      , m_building3D(bld3D)
      , m_modified(false)
      , m_status(UNSELECTED)
      , m_currentTextureSurface(FRONT_TEXTURE)
    {
      m_textureName.format("tex%7.7d.png", m_objectId);
    }

    BuildingObject(const bstream& bs)
      : m_modified(false)
      , m_status(UNSELECTED)
      , m_currentTextureSurface(FRONT_TEXTURE) 
    {
      read(bs);
      // 既存のオブジェクトIDが大きかったらそれ以上にする
      if (m_objectId >= m_lastId) {
        m_lastId = m_objectId + 1;
      }
    }


    void setStatus(Status st) {
      m_status = st;
      switch (m_status) {
      case UNSELECTED:
        m_building3D->setColor(ColorRGBA(255,200,200,150));
        break;
      case SELECTED:
        m_building3D->setColor(ColorRGBA(255,0,0,255));
        m_building3D->enableSolid(false);
        m_building3D->enableBorder(true);
        break;
      }

    }
    Status getStatus() const { return m_status; }

    bool hitCheckVertex2D(const Vector2D& vtx, const Vector2D& range) {
      return m_building3D->hitCheckVertex2D(vtx, range);
    }


    void setTexture(const string& fname) {
      m_building3D->setTexture2D(TextureManager::LoadTexture(fname));
      m_textureName = fname;
    }
    void setTexture2D(gobj::Texture2D::ptr tex) {
      m_building3D->setTexture2D(tex);
      TextureManager::SetNewTexture(m_textureName, tex);
    }

    void setTexture2D() {
      TextureManager::SetNewTexture(m_textureName, m_building3D->getTexture2D());
    }

    // 建物の位置の移動
    void addOrigin(const Vector3Dd& v) {
      m_building3D->setOrigin(m_building3D->getOrigin() + v);
      m_modified = true;
    }
    void addScale(const Vector3D& v) {
      m_building3D->setScale(m_building3D->getScale() + v);
      m_modified = true;
    }

    // 選択されている頂点の移動
    void moveVertex(const Vector2D& move) {
      if (m_building3D->moveVertex(move)) {
        m_modified = true;
      }
    }

    // 分割面の移動
    void moveCutSurface(const Vector2D& move) {
      m_building3D->moveCutSurface(move);
    }

    // 建物の分割
    // 分割後にできたオブジェクトを返す
    BuildingObject::ptr doCutBuilding() {
      BuildingObject::ptr newbldg;
      switch(m_type) {
      default:
      case SIMPLE_BUILDING:
        break;
      case FREE_BUILDING:
        newbldg = BuildingObject::ptr(new BuildingObject(FREE_BUILDING));
        m_building3D->doCutBuilding(newbldg->m_building3D);
        newbldg->setParentObject(getObjectId());
      }
      return newbldg;
    }
    BuildingObject::ptr addCubeObject() {
      BuildingObject::ptr newbldg;
      switch(m_type) {
      default:
      case SIMPLE_BUILDING:
        break;
      case FREE_BUILDING:
        {
          FreeBuilding3D::ptr bld3D = dynamic_pointer_cast<FreeBuilding3D>(m_building3D);
          // 右上の頂点を取得
          Vector3D frt = bld3D->getBbVertex(FreeBuilding3D::FRT);
          // 中心から右上の頂点のサイズの立方体を生成
          FreeBuilding3D::ptr new3D(new FreeBuilding3D(FreeBuilding3D::Cube, frt));
          new3D->setOrigin(Vector3Dd(bld3D->getOrigin().x, bld3D->getOrigin().y, frt.z));
          // BuildingObjectを生成
          newbldg = BuildingObject::ptr(new BuildingObject(FREE_BUILDING, new3D, getObjectId()));
        }
        break;
      }
      return newbldg;
    }
    

    void read(const bstream& bs) {
      bstream tmp;
      bs >> tmp;
      tmp >> m_version;
      tmp >> m_objectId;
      tmp >> m_type;
      tmp >> m_parentObjectId;
      switch (m_type) {
      case SIMPLE_BUILDING:
        m_building3D = IBuilding3D::ptr(new SimpleBuilding3D);
        tmp >> m_textureName;
        m_building3D->deserialize(tmp);
        setTexture(m_textureName);
        break;
      case FREE_BUILDING:
        m_building3D = IBuilding3D::ptr(new FreeBuilding3D);
        tmp >> m_textureName;
        m_building3D->deserialize(tmp);
        setTexture(m_textureName);
        //DEBUGLOG("FREE_BUILDING Texture:%s\n", m_textureName.c_str());
        break;
      default:
        ERRORLOG("BuildingObject Type error\n");
        break;
      }
    }
    void write(bstream& bs) {
      bstream tmp;
      tmp << m_version;
      tmp << m_objectId;
      tmp << static_cast<uint32_t>(m_type);
      tmp << m_parentObjectId;
      tmp << m_textureName;
      //TRACE("WriteTexture:%s\n", m_textureName.c_str());
      m_building3D->serialize(tmp);
      bs << tmp;
    }

    void setRenderingMatrix(const Matrix& m) {
      m_matrix = m;
    }

    // 現在選択されているテクスチャの面を得る
    TextureSurface getCurrentTextureSurface() const { return m_currentTextureSurface; }
    // 現在選択されているテクスチャの面を設定する
    void setCurrentTextureSurface(TextureSurface suf) { m_currentTextureSurface = suf; }
    void setCurrentTextureSurface(const Vector2D& v) {
      for (uint32_t suf = 0; suf < NO_TEXTURE; ++suf) {
        if (getCurrentTextureCoord(static_cast<TextureSurface>(suf)).isInside(v.x, v.y)) {
          m_currentTextureSurface = static_cast<TextureSurface>(suf);
          return;
        }
      }
    }


    // 現在選択されているテクスチャの座標を得る (左上が原点)
    RectF32 getCurrentTextureCoord(TextureSurface suf = NO_TEXTURE) const {
      if (suf == NO_TEXTURE) suf = m_currentTextureSurface;
      RectF32 rect(0,0,0,0);
      switch(suf) {
      case TOP_TEXTURE:
        rect.left = 0.0f;
        rect.width = 0.5f;
        rect.height = 160.0f/512.0f;
        rect.top = 0; //1.0f - rect.height;
        break;
      case FRONT_TEXTURE:
        rect.left = 0.5f;
        rect.width = 0.5f;
        rect.top = 160.0f/512.0f; //0.0f;
        rect.height = (512.0f - 160.0f)/512.0f;
        break;
      case SIDE_TEXTURE:
        rect.left = 0.0f;
        rect.width = 0.5f;
        rect.top = 160.0f/512.0f; // 0.0f;
        rect.height = (512.0f - 160.0f)/512.0f;
        break;
      default:
        break;
      }
      return rect;
    };



    // getter/setter

    void getDrawPrimitives(vector<DrawPrimitive>& primitives) {
      m_building3D->getDrawPrimitives(primitives);
    }

    void setCutBuildingMode(bool enable) { 
      m_building3D->setCutBuildingMode(enable);
    }

    void setParentObject(uint32_t id) {
      m_parentObjectId = id;
    }

    uint32_t getParentObject() const {
      return m_parentObjectId;
    }

    void setDirection(float32_t r) {
      m_building3D->setDirection(m_building3D->getDirection() + r);
      m_modified = true;
    }
    void setBbDirection(float32_t r) {
      m_building3D->setBbDirection(m_building3D->getBbDirection() + r);
      m_modified = true;
    }

    gobj::Texture2D::ptr getTexture2D() {
      return m_building3D->getTexture2D();
    }

    const Vector3Dd& getOrigin() const {
      return m_building3D->getOrigin();
    }


    IBuilding3D::ptr getObject() { return m_building3D; }

    void setObjectId(uint32_t id) { m_objectId = id; }
    uint32_t getObjectId() const { return m_objectId; }

    // flags
    void setModified(bool enable = true) { m_modified = enable; }
    bool isModified() const { return m_modified; }
    void clearMidified() { m_modified = false; }

    static void SetLastObjectId(uint32_t id) {
      if (m_lastId < id) {
        m_lastId = id;
      }
    }
    static uint32_t GetLastObjectId() {
      return m_lastId;
    }
  private:
    uint32_t m_version;
    uint32_t m_objectId;
    uint32_t m_parentObjectId;
    BuildingType m_type;
    IBuilding3D::ptr m_building3D;
    mstring m_textureName;
    bool m_modified;
    Matrix m_matrix;
    Status m_status;
    TextureSurface m_currentTextureSurface;
    static uint32_t m_lastId;
 };

} // namespace

#endif

