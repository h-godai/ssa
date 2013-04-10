// AeroPhotoObject.h
//
// 航空写真画像、2D系座標、3Dオブジェクトのデータクラス
//

#ifndef AeroPhotoObject_H
#define AeroPhotoObject_H

#ifndef STDAFX_H
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#endif
#include "Graphics/GraphicsObject.h"
#include "PlatformDependent/Sys/ISystemInterface.h"
#include "Graphics/Image/ImageBuffer.hpp"
#include "Graphics/Image/JpegImage.hpp"
#include "ShpFile.hpp"
#include "Math/CameraPosition.h"
#include "Util/ustring.hpp"
#include "Importer.hpp"
#include "Math/Geometric.hpp"

namespace app {
  using namespace ts;
  using namespace ts::gobj;
  using namespace ts::math;
  using boost::shared_ptr;
  using std::vector;
  using std::string;

  const uint32_t MapObjTextureWidth = 1024;
  const uint32_t MapObjTextureHeight = 1024;

  const RectF64  MapClipRect(-9824.0, -39946.0,  10000.0, 10000.0); // 有効な領域


  // 地図の方向
  enum MapDirection {
    MAPDIR_NORTH, // 北向きの撮影した写真 (カメラは南)
    MAPDIR_SOUTH,
    MAPDIR_EAST,
    MAPDIR_WEST,
    MAPDIR_TOP,    // 北を上に見下ろした写真
    MAPDIR_UNKNOWN,
    MAPDIR_MAX
  }; 

  class AeroPhotoObject {
  public:
    typedef shared_ptr<AeroPhotoObject> ptr;

    enum Status {
      Visible,
      Hide,
      WireFrame,
      Unload
    };

    AeroPhotoObject()
      : m_mapObj3D(new Group)
      , m_rect3D(new Rectangle3D)
      , m_id(0)
      , m_status(WireFrame)
      , m_direction(MAPDIR_TOP)
    {
    }
    void initialize(ShpFile::Polygon* pg = 0) {
      if (pg != 0) {
        m_vertex[0] = Vector3Dd(pg->points[1].x, pg->points[1].y, 0.0f);
        m_vertex[1] = Vector3Dd(pg->points[0].x, pg->points[0].y, 0.0f);
        m_vertex[2] = Vector3Dd(pg->points[3].x, pg->points[3].y, 0.0f);
        m_vertex[3] = Vector3Dd(pg->points[2].x, pg->points[2].y, 0.0f);
      }
      m_rect3D->setColor(ColorRGBA(100,100,100,255));
      m_rect3D->setBorderColor(ColorRGBA(100,100,100,255));
      m_rect3D->setVertexes(m_vertex);
      m_mapObj3D->addChild(m_rect3D);
    }

    void loadImage() {
      if (m_image.get() == 0) {
        sys::PushMouseCursor(sys::WAIT_CURSOR);
        INFOLOG("LoadAeroPhoto: %s\n", m_imagePathName.c_str());
        m_image = boost::dynamic_pointer_cast<ImageBufferARGB>(JpegImage().deCompress(m_imagePathName, ImageBuffer::A8R8G8B8));
        if (m_image.get() != 0) {
          m_texImage = shared_ptr<ImageBufferARGB>(new ImageBufferARGB(MapObjTextureWidth, MapObjTextureHeight));
          m_texImage->copyFrom(*m_image);
          m_texture = shared_ptr<Texture2D>(new Texture2D);
          m_texture->setImageBuffer(m_texImage);
          m_rect3D->setTexture2D(m_texture);
        }
        else {
          TRACE("AeroPhotoObject::loadImage(%s) failed\n", m_imagePathName.c_str());
        }
        sys::PopMouseCursor();
      }
    }

    void setZoffset(float32_t offset) {
      m_mapObj3D->setZoffset(offset);
    }

    void setStatus(Status st) {
      switch(m_status = st) {
      case Visible:
        loadImage();
        m_mapObj3D->enable();
        m_rect3D->enableTexture(false);
        m_rect3D->enableBorder(false);
        break;
      case Hide:
        m_mapObj3D->disable();
        break;
      case WireFrame:
        m_mapObj3D->enable();
        m_rect3D->enableTexture(false);
        m_rect3D->enableBorder(true);
      default:
        break;
      }
    }

    void setFocus(bool enable) {
      m_rect3D->enableBorder(true);
      if (enable) {
        m_rect3D->setLineWidth(2.0f);
        m_rect3D->setBorderColor(ColorRGBA(255,255,0,255));
      }
      else {
        m_rect3D->setLineWidth(1.0f);
        m_rect3D->setBorderColor(ColorRGBA(50,50,50,255));
      }
    }



    Status getStatus() const { m_status; }

    uint32_t getId() const { return m_id; }
    void setId(uint32_t id) { m_id = id; }

    shared_ptr<Rectangle3D> getRect3D() { return m_rect3D; }
    Group::ptr& getGraphicsObject() { return m_mapObj3D; }
    const RectVector3Dd& getVertex() const { return m_vertex; }
    Vector3D getVertex(int n) const { return m_vertex[n]; }
    shared_ptr<ImageBufferARGB> getImage() { return m_image; }
    void setDbInfo(DBaseFile::DataListPtr info) { m_dbInfo = info; }
    DBaseFile::DataListPtr getDbInfo() { return m_dbInfo; }
    shared_ptr<Texture2D> getTexture() { return m_texture; }



    // 航空写真の四隅の座標を得る
    const RectVector3Dd& getCorners() const {
      return m_vertex;
    }

    // 座標がマップ上にあるか調べる
    bool isInside(const Vector3Dd& pos) {
      int s1 = SideCheck2D(m_vertex[1], m_vertex[0], Vector2Dd(pos.x, pos.y));
      s1 += SideCheck2D(m_vertex[2], m_vertex[1], Vector2Dd(pos.x, pos.y));
      s1 += SideCheck2D(m_vertex[3], m_vertex[2], Vector2Dd(pos.x, pos.y));
      s1 += SideCheck2D(m_vertex[0], m_vertex[3], Vector2Dd(pos.x, pos.y));
      return (s1 == 4);
    }


    void setCameraPos(const CameraPosition& camera) { m_camera = camera; }
    const CameraPosition& getCameraPos() const { return m_camera; }

    void setImagePathName(const string& name) { m_imagePathName = name; }

    void setName(const string& name) { m_name = name; }
    const string& getName() const {
        return m_name; 
    }
    const string& getImagePath() const {
        return m_imagePathName;
    }

    void write(bstream& bs) {
      mstring fn(m_imagePathName);
      if (fn.leftMatch(Importer::GetRootDirectory())){
        // rootからの相対パスにする
        m_imagePathName = fn.substr(Importer::GetRootDirectory().length());
      }
      bstream out;
      out << m_name;
      out << m_imagePathName;
      out << m_vertex;
      out << m_camera;
      out << m_direction;
      out << m_originalCamera;
      out << m_originalCameraAngle;
      out << m_originalCameraElevation;
      bs << out;
    }
    void read(const bstream& bs) {
#ifdef OLDMOC
      bs >> m_name;
      bs >> m_imagePathName;
      bs >> m_vertex;
      bs >> m_camera;
#else
      bstream in;
      in.deserialize(bs);
      in >> m_name;
      in >> m_imagePathName;
      in >> m_vertex;
      in >> m_camera;
      in >> m_direction;
      in >> m_originalCamera;
      in >> m_originalCameraAngle;
      in >> m_originalCameraElevation;
#endif
      if (m_imagePathName[0] != '/' && m_imagePathName[1] != ':') {
        // 相対パスだったらRootDirectoryを加える
        m_imagePathName = Importer::GetRootDirectory() + m_imagePathName;
      }
    }

    Vector3Dd& originalCamera() { return m_originalCamera; }
    const Vector3Dd& getOriginalCamera() const { return m_originalCamera; }
    Vector3Dd& originalCameraAngle() { return m_originalCameraAngle; }
    void setOriginalCameraElevation(double ev) { m_originalCameraElevation = ev; }
    double getOriginalCameraElevation() const { return m_originalCameraElevation; }

    void setDirection(MapDirection dir) { m_direction = dir; }
    MapDirection getDirection() const { return m_direction; }

  private:
    string m_name;                    // 名称(通常は画像ファイル名)
    RectVector3Dd m_vertex;               // 写真の頂点座標
    shared_ptr<Group> m_mapObj3D;         // 描画オブジェクトグループ
    shared_ptr<Rectangle3D> m_rect3D;     // 描画オブジェクト
    shared_ptr<ImageBufferARGB> m_image;   // ビットマップイメージ
    shared_ptr<ImageBufferARGB> m_texImage;   // テクスチャ用ビットマップイメージ
    uint32_t m_id;
    Status m_status;
    DBaseFile::DataListPtr m_dbInfo;  // dbfファイルのデータ
    CameraPosition m_camera;
    string m_imagePathName;            // イメージファイル名。データれィレクトリからの相対
    shared_ptr<Texture2D> m_texture;
    MapDirection m_direction; // カメラの向き
    Vector3Dd m_originalCamera;   // 元データのカメラ位置情報
    Vector3Dd m_originalCameraAngle; // 元データのカメラ向き
    double    m_originalCameraElevation; // カメラ位置の標高
  };



} // namespace

namespace ts {
  using namespace app;
  // bstreamへの書き出し
  template<>
  inline bstream& operator << (bstream& bs, AeroPhotoObject& mo) {
    mo.write(bs);
    return bs;
  }
  // bstreamからの読み込み
  template<>
  inline const bstream& operator >> (const bstream& bs, AeroPhotoObject& mo) {
    mo.read(bs);
    return bs;
  }
}


#endif
