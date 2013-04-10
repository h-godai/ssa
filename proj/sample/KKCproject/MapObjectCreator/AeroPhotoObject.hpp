// AeroPhotoObject.h
//
// �q��ʐ^�摜�A2D�n���W�A3D�I�u�W�F�N�g�̃f�[�^�N���X
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

  const RectF64  MapClipRect(-9824.0, -39946.0,  10000.0, 10000.0); // �L���ȗ̈�


  // �n�}�̕���
  enum MapDirection {
    MAPDIR_NORTH, // �k�����̎B�e�����ʐ^ (�J�����͓�)
    MAPDIR_SOUTH,
    MAPDIR_EAST,
    MAPDIR_WEST,
    MAPDIR_TOP,    // �k����Ɍ����낵���ʐ^
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



    // �q��ʐ^�̎l���̍��W�𓾂�
    const RectVector3Dd& getCorners() const {
      return m_vertex;
    }

    // ���W���}�b�v��ɂ��邩���ׂ�
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
        // root����̑��΃p�X�ɂ���
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
        // ���΃p�X��������RootDirectory��������
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
    string m_name;                    // ����(�ʏ�͉摜�t�@�C����)
    RectVector3Dd m_vertex;               // �ʐ^�̒��_���W
    shared_ptr<Group> m_mapObj3D;         // �`��I�u�W�F�N�g�O���[�v
    shared_ptr<Rectangle3D> m_rect3D;     // �`��I�u�W�F�N�g
    shared_ptr<ImageBufferARGB> m_image;   // �r�b�g�}�b�v�C���[�W
    shared_ptr<ImageBufferARGB> m_texImage;   // �e�N�X�`���p�r�b�g�}�b�v�C���[�W
    uint32_t m_id;
    Status m_status;
    DBaseFile::DataListPtr m_dbInfo;  // dbf�t�@�C���̃f�[�^
    CameraPosition m_camera;
    string m_imagePathName;            // �C���[�W�t�@�C�����B�f�[�^��B���N�g������̑���
    shared_ptr<Texture2D> m_texture;
    MapDirection m_direction; // �J�����̌���
    Vector3Dd m_originalCamera;   // ���f�[�^�̃J�����ʒu���
    Vector3Dd m_originalCameraAngle; // ���f�[�^�̃J��������
    double    m_originalCameraElevation; // �J�����ʒu�̕W��
  };



} // namespace

namespace ts {
  using namespace app;
  // bstream�ւ̏����o��
  template<>
  inline bstream& operator << (bstream& bs, AeroPhotoObject& mo) {
    mo.write(bs);
    return bs;
  }
  // bstream����̓ǂݍ���
  template<>
  inline const bstream& operator >> (const bstream& bs, AeroPhotoObject& mo) {
    mo.read(bs);
    return bs;
  }
}


#endif
