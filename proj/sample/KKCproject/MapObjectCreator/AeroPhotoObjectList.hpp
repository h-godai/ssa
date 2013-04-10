// AeroPhotoObjectList.h



#ifndef AeroPhotoObjectList_H
#define AeroPhotoObjectList_H

#ifndef STDAFX_H
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <exception>
#endif
#include "Graphics/GraphicsObject.h"
#include "AeroPhotoObject.hpp"
#include "Util/bstream.hpp"
#include "AppStatusManager.hpp"

namespace app {

  using boost::shared_ptr;
  using std::vector;
  using ts::bstream;
  using std::for_each;


  class AeroPhotoObjectList {
    static const uint32_t VersionNumber = 1;
  public:
    static const uint32_t InvalidId = 0xffffffff;

    AeroPhotoObjectList()
      : m_currentAeroPhotoObjectId(0)
      , m_aeroPhotoObjectId(0)
      , m_focusedObjectId(InvalidId)
    {
    }


    // AeroPhotoObjectを追加し、レコード番号を返す
    uint32_t addAeroPhotoObject(AeroPhotoObject::ptr& ptr) {
      ptr->setId(m_aeroPhotoObjectId++);
      m_aeroPhotoObject.push_back(ptr);
      return ptr->getId(); // レコード番号
    }

    // マウスカーソルが指しているマップオブジェクトを探す
    // 無い場合はnull
    uint32_t findByMapPosition(const Vector3Dd& pos, MapDirection mapDir, bool setfocus = false) {
      uint32_t foundMapId = InvalidId;
      double distance = 999999.0;
      AeroPhotoObject::ptr mapObj;
      if (m_aeroPhotoObject.size() == 0) return foundMapId; 
      for (uint32_t id = 0; id < m_aeroPhotoObject.size(); ++id) {
        mapObj = m_aeroPhotoObject[id];
        if (setfocus) { mapObj->setFocus(false); }
        if (mapObj->getDirection() != mapDir) continue;
        if (mapObj->isInside(pos)) {
          // マップ上にカーソルがある場合は、一番近いマップを選別する
          Vector3Dd center = CameraPositionCalculator::getCenterPoint(mapObj->getCorners());
          if (distance > pos.distance(center)) {
            distance = pos.distance(center);
            if (setfocus) {
              if (foundMapId != InvalidId) {
                m_aeroPhotoObject[foundMapId]->setFocus(false);
              }
              mapObj->setFocus(true);
              m_focusedObjectId = id;
            }
            foundMapId = id;
          }
        }
      }
      return foundMapId;
    }


    void setStatus(uint32_t record, AeroPhotoObject::Status status) {
      m_aeroPhotoObject[record]->setStatus(status);
    }

    const AeroPhotoObject::ptr getAeroPhotoObject(uint32_t mapId) const {
      return m_aeroPhotoObject[mapId];
    }
    AeroPhotoObject::ptr getAeroPhotoObject(uint32_t mapId) {
      return m_aeroPhotoObject[mapId];
    }

    void setCurrentAeroPhotoObject(uint32_t mapId) {
      if (m_currentAeroPhotoObjectId >= m_aeroPhotoObject.size()) throw std::out_of_range("AeroPhotoObjectList::setCurrentAeroPhotoObject");
      if (m_currentAeroPhotoObjectId != InvalidId) {
        m_aeroPhotoObject[m_currentAeroPhotoObjectId]->setStatus(AeroPhotoObject::WireFrame);
      }
      m_currentAeroPhotoObjectId = mapId;
      m_aeroPhotoObject[mapId]->setStatus(AeroPhotoObject::Visible);
      AppStatusManager::SetCurrentAeroPhotoObject(m_aeroPhotoObject[mapId]);
    }
#if 0
    // @throw std::out_of_range
    const AeroPhotoObject::ptr getCurrentAeroPhotoObject() {
      if (m_currentAeroPhotoObjectId >= m_aeroPhotoObject.size()) return AeroPhotoObject::ptr();
      return m_aeroPhotoObject.at(m_currentAeroPhotoObjectId);
    }
    const AeroPhotoObject::ptr getCurrentAeroPhotoObject() const {
      if (m_currentAeroPhotoObjectId >= m_aeroPhotoObject.size()) return AeroPhotoObject::ptr();
      return m_aeroPhotoObject.at(m_currentAeroPhotoObjectId);
    }
#endif

    size_t getAeroPhotoObjectCount() const { return m_aeroPhotoObject.size(); }


    void setDataDir(const string& dir) { m_dataDir = dir; }
    const string& getDataDir() const { return m_dataDir; }

    void write(ostream& os) {
      bstream bs;
      write(bs);
      os << bs;
    }

    void write(bstream& bs) {
      uint32_t count = static_cast<uint32_t>(m_aeroPhotoObject.size());
      bs << VersionNumber;
      bs << count;
      BOOST_FOREACH(AeroPhotoObject::ptr& mo, m_aeroPhotoObject) {
        bs << *mo;
      }
    }

    void read(istream& is) {
      bstream bs;
      bs << is;
      read(bs);
    }

    void read(const bstream& bs) {
      uint32_t count;
      bs >> count;
      if (count != VersionNumber) return;
      bs >> count;
      for (uint32_t idx = 0; idx < count; ++idx) {
        AeroPhotoObject::ptr mo(new AeroPhotoObject);
        bs >> *mo;
        if (MapClipRect.isInside(mo->getVertex()[0].x, mo->getVertex()[0].y)) {
          addAeroPhotoObject(mo);
        }
      }
    }

    AeroPhotoObject::ptr findAeroPhotoObject(const string& name) {
      BOOST_FOREACH(AeroPhotoObject::ptr& mo, m_aeroPhotoObject) {
        if (mo->getName() == name) {
          return mo;
        }
      }
      return AeroPhotoObject::ptr();
    }

    void clear() {
      m_aeroPhotoObject.clear();
      m_aeroPhotoObjectId = 0;
    }

    bool isEmpty() const { return m_aeroPhotoObject.empty(); }

    // setter, getter

    uint32_t getFocusedObjectId() const { return m_focusedObjectId; }

  private:
    vector<AeroPhotoObject::ptr> m_aeroPhotoObject;
    uint32_t m_currentAeroPhotoObjectId;
    uint32_t m_focusedObjectId;
    string m_dataDir;
    uint32_t m_aeroPhotoObjectId;

  };

} // namespace

#endif
