// BuildingObjectList
//
// 地上にある建築物のデータリスト
//
//
#ifndef BUILDINGOBJECTLIST_HPP
#define BUILDINGOBJECTLIST_HPP

#ifndef STDAFX_H
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#endif
#include "BuildingObject.hpp"
#include "Graphics/GraphicsObject.h"
#include "AppStatusManager.hpp"

namespace app {
  using boost::shared_ptr;
  using std::vector;
  using namespace ts;


  class BuildingObjectList {
  public:
    typedef shared_ptr<BuildingObjectList> ptr;
    BuildingObjectList()
      : m_gobjRoot(new gobj::Group)
      , m_enableWireFrame(false)
    {
    }

    void clear() {
      m_gobjRoot = gobj::Group::ptr(new gobj::Group);
      m_list.clear();
      m_currentObject = BuildingObject::ptr();
    }

    size_t getObjectCount() const { return m_list.size(); }


    void addBuildingObject(BuildingObject::ptr& bldg) {
//      bldg->setObjectId(static_cast<uint32_t>(m_list.size()));
      m_list.push_back(bldg);
      m_gobjRoot->addChild(bldg->getObject());
    }

    void setChildObject(gobj::Group::ptr& grp, BuildingObject::ptr& bldg) {
      BOOST_FOREACH(BuildingObject::ptr& b, m_list) {
        if (b->getParentObject() == bldg->getObjectId()
          || bldg->getParentObject() == b->getObjectId()) {
          grp->addChild(b->getObject());
        }
      }
    }

    BuildingObject::ptr getCurrentObject() {
      return m_currentObject;
    }

    void setCurrentObject(BuildingObject::ptr& obj) {
      if (m_currentObject.get() != 0) {
        unsetCurrentObject(m_currentObject);
      }
      if (obj.get() == 0) return;
      m_currentObject = obj;
      obj->setStatus(BuildingObject::SELECTED);
      AppStatusManager::SetCurrentBuildingObject(obj);
    }

    void setCurrentObject(uint32_t idx) {
      if (idx >= m_list.size()) {
        throw std::out_of_range("BuildingObjectList::setCurrentObject");
      }
      setCurrentObject(m_list[idx]);
    }

    BuildingObject::ptr getObject(uint32_t idx) {
      if (idx >= m_list.size()) {
        throw std::out_of_range("BuildingObjectList::setCurrentObject");
      }
      return m_list[idx];
    }

    void unsetAllObject() {
      BOOST_FOREACH(BuildingObject::ptr& b, m_list) {
        unsetCurrentObject(b);
      }
      m_currentObject = BuildingObject::ptr();
      AppStatusManager::SetCurrentBuildingObject(m_currentObject);
    }

    void unsetCurrentObject(BuildingObject::ptr& obj) {
      obj->setStatus(BuildingObject::UNSELECTED);
      obj->getObject()->enableSolid(!m_enableWireFrame);
      obj->getObject()->enableBorder(m_enableWireFrame);
    }

    // 2D座標系からカレントオブジェクトをセットする
    // セットできたらtrueを返す
    bool setCurrentObjectFor2D(const Vector2D& xy) {
      // まずカレントオブジェクトを調べる
      bool hitCurrent = false;
      if (m_currentObject.get() != 0) {
        if (m_currentObject->getObject()->isPointInRect2D(xy)) {
          hitCurrent =  true;
        }
      }

      BuildingObject::ptr hitObj;
      BOOST_FOREACH(BuildingObject::ptr& b, m_list) {
        if (b->getObject()->isPointInRect2D(xy)) {
          if (hitCurrent) {
            if (b->getParentObject() == m_currentObject->getObjectId()) {
              hitObj = b;
            }
          }
          else {
            hitObj = b;
          }
        }
        unsetCurrentObject(b);
      }
      if (isNull(hitObj)) {
        if (hitCurrent) {
          setCurrentObject(m_currentObject);
          return true; // カレントのまま
        }
        else {
          // カレントはない
          m_currentObject = BuildingObject::ptr();
          AppStatusManager::SetCurrentBuildingObject(m_currentObject);
          return false;
        }
      }
      else {
        setCurrentObject(hitObj);
        return true;
      }
    }

    void set2DMatrix(const Matrix& mmm) {
      BOOST_FOREACH(BuildingObject::ptr& bldg, m_list) {
        IBuilding3D::ptr bldg3D = bldg->getObject();
        bldg3D->makeRectangle2D(mmm);
      }
    }

    // 建物分割モード
    void setCutBuildingMode(bool enable) { 
      BOOST_FOREACH(BuildingObject::ptr& bldg, m_list) {
        if (enable && bldg.get() == m_currentObject.get()) {
          bldg->setCutBuildingMode(true);
        }
        else {
          bldg->setCutBuildingMode(false);
        }
      }
    }


    // デバッグ用
    void setScaleAll(const Vector3D& scale) {
      BOOST_FOREACH(BuildingObject::ptr& bldg, m_list) {
        IBuilding3D::ptr bldg3D = bldg->getObject();
        bldg3D->setScale(scale);
      }
    }
    // デバッグ用
    void setOriginAll(const Vector3Dd& origin) {
      BOOST_FOREACH(BuildingObject::ptr& bldg, m_list) {
        IBuilding3D::ptr bldg3D = bldg->getObject();
        bldg3D->setOrigin(origin);
      }
    }


    Group::ptr getGObjRoot() {
      return m_gobjRoot;
    }

    void setWireFrame(bool enable = true) {
      m_enableWireFrame = enable;
      BOOST_FOREACH(BuildingObject::ptr& b, m_list) {
        b->getObject()->enableSolid(!enable);
        b->getObject()->enableBorder(enable);
        unsetCurrentObject(b);
      }
     // setCurrentObject(m_currentObject);
    }

    void read(istream& is) {
      bstream bs;
      bs.deserialize(is);
      read(bs);
    }

    void read(const bstream& bs) {
      clear();
      uint32_t count;
      bs >> count;
      while (count-- > 0) {
        BuildingObject::ptr b(new BuildingObject(bs));
        addBuildingObject(b);
      }
    }

    void write(ostream& os) {
      bstream bs;
      write(bs);
      bs.serialize(os);
    }

    void write(bstream& bs) {
      bs << static_cast<uint32_t>(m_list.size());
      BOOST_FOREACH(BuildingObject::ptr& b, m_list) {
        b->write(bs);
      }
    }




  private:
    BuildingObject::ptr m_currentObject;
    gobj::Group::ptr m_gobjRoot;
    vector<BuildingObject::ptr> m_list;
    bool m_enableWireFrame;
  };


} // namespace

#endif
