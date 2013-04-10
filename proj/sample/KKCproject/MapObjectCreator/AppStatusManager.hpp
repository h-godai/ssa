// AppStatusManager
//
// �A�v���P�[�V�����̏�Ԃ��Ǘ�����N���X

#ifndef APPSTATUSMANAGER_HPP
#define APPSTATUSMANAGER_HPP

#ifndef STDAFX_H
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#endif


#include "AeroPhotoObject.hpp"
#include "BuildingObject.hpp"

namespace app {

  using std::string;
  using std::vector;
  using boost::shared_ptr;

  // �A�v���P�[�V�������[�h
  enum AppMode {
    APP_MAP_VIEW_MODE,
    APP_EDIT_BUILDING_MODE,
    APP_CALC_CAMERAPOS_MODE,
    APP_EDIT_TEXTURE_MODE
  };

  // AppStatus���ω������Ƃ��ɃR�[���o�b�N�����C�x���g�n���h���̃C���^�[�t�F�C�X��`
  class IAppStatusEventHandler {
  public:
    typedef shared_ptr<IAppStatusEventHandler> ptr;
    virtual ~IAppStatusEventHandler() {}
    // �J�����gAeroPhotoObject���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
    virtual void onSetCurrentAeroPhotoObject(AeroPhotoObject::ptr) {} 
    // �J�����gBuildingObject���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
    virtual void onSetCurrentBuildingObject(BuildingObject::ptr) {} 
    // StatusText���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
    virtual void onSetStatusText(const string&) {}
    // AppMode���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
    virtual void onSetAppMode(AppMode ) {}
    // �}�b�v�̕��p���ύX���ꂽ
    virtual void onSetMapDirection(MapDirection) {}

    // ���ꃁ�\�b�h���ŌĂяo��
    void onSetObject(AeroPhotoObject::ptr& obj) { onSetCurrentAeroPhotoObject(obj); }
    void onSetObject(BuildingObject::ptr& obj) { onSetCurrentBuildingObject(obj); }
    void onSetObject(const string& obj) { onSetStatusText(obj); }
    void onSetObject(AppMode& obj) { onSetAppMode(obj); }
    void onSetObject(MapDirection& obj) { onSetMapDirection(obj);}
  };



  // �A�v���P�[�V�����X�e�[�^�X�}�l�[�W��
  class AppStatusManager {
  private:
    // �C�x���g���X�g�N���X
    struct EventList {
      // for_each�p�̊֐��I�u�W�F�N�g
      template <typename T>
      struct OnSetObject {
        T& obj;
        OnSetObject(T& o) : obj(o) {}
        void operator() (IAppStatusEventHandler*& ev) {
          ev->onSetObject(obj);
        }
      };
      template <typename T>
      void onSetObject(T& obj) {
        for_each(m_events.begin(), m_events.end(), OnSetObject<T>(obj));
      }

      void push_back(IAppStatusEventHandler* eh) {
        m_events.push_back(eh);
      }

      void remove(IAppStatusEventHandler* eh) {
        vector<IAppStatusEventHandler*>::iterator ite = find(m_events.begin(), m_events.end(), eh);
        if (ite != m_events.end()) {
          m_events.erase(ite);
        }
      }
      vector<IAppStatusEventHandler*> m_events;
    };

  public:
    AppStatusManager()
      : m_mode(APP_MAP_VIEW_MODE) 
      , m_mapDirection(MAPDIR_NORTH)
    {
      m_currentAeroPhotoObject.resize(MAPDIR_UNKNOWN+1);
    }

    // AppStatus�̓V���O���g��
    static AppStatusManager& GetInstance() {
      static AppStatusManager myInstance;
      return myInstance;
    }

    static bool SetAppMode(AppMode mode) {
      switch(mode) {
      case APP_MAP_VIEW_MODE:
      case APP_EDIT_BUILDING_MODE:
      case APP_CALC_CAMERAPOS_MODE:
        break;
      case APP_EDIT_TEXTURE_MODE:
        if (GetInstance().m_currentBuildingObject.get() == 0) {
          return false;
        }
        break;
      }
      GetInstance().m_mode = mode;
      GetInstance().m_eventList.onSetObject(mode);
      return true;
    }

    static AppMode GetAppMode() {
      return GetInstance().m_mode;
    }

    static void SetMapDirection(MapDirection dir) {
      lock();
      GetInstance().m_mapDirection = dir;
      unlock();
      GetInstance().m_eventList.onSetObject(dir);
    }

    static MapDirection GetMapDirection() {
      return GetInstance().m_mapDirection;
    }


    static void SetCurrentAeroPhotoObject(AeroPhotoObject::ptr mapObj) {
      lock();
      GetInstance().m_currentAeroPhotoObject[GetMapDirection()] = mapObj; 
      unlock();
      GetInstance().m_eventList.onSetObject(mapObj);
    }

    static AeroPhotoObject::ptr getCurrentAeroPhotoObject() { 
      return GetInstance().m_currentAeroPhotoObject[GetMapDirection()]; 
    }

    static void SetCurrentBuildingObject(BuildingObject::ptr bldg) {
      lock();
      GetInstance().m_currentBuildingObject = bldg; 
      unlock();
      GetInstance().m_eventList.onSetObject(bldg);
    }

    static BuildingObject::ptr GetCurrentBuildingObject() {
      return GetInstance().m_currentBuildingObject; 
    }

    static void SetStatusText(const string& str) {
      lock();
      GetInstance().m_statusText = str;
      unlock();
      GetInstance().m_eventList.onSetObject(str);
    }
    
    static void AddEventHandler(IAppStatusEventHandler* eh) {
      GetInstance().m_eventList.push_back(eh);
    }

    static void RemoveEventHandler(IAppStatusEventHandler* eh) {
      GetInstance().m_eventList.remove(eh);
    }

    static void lock() {}
    static void unlock() {}

  private:
    vector<AeroPhotoObject::ptr> m_currentAeroPhotoObject;
    BuildingObject::ptr m_currentBuildingObject;
    string m_statusText;
    EventList m_eventList;
    AppMode m_mode;
    MapDirection m_mapDirection;
  };

}

#endif
