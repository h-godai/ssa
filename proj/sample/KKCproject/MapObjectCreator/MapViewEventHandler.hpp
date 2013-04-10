// MapViewEventHandler.hpp
//
// �}�b�v�r���[���[�h�C�x���g�n���h��
//

#ifndef MAPVIEWEVENTHANDLER_HPP
#define MAPVIEWEVENTHANDLER_HPP

#include "MouseEventHandler.hpp"
#include "AeroPhotoObjectList.hpp"

namespace app {
  extern Vector3D gldOrigin;

  // �r���[���[�h
  class MapViewEventHandler : public MouseEventHandler {
    AeroPhotoObjectList& m_mapList;
  public:
    typedef shared_ptr<MapViewEventHandler> ptr;
    MapViewEventHandler(ViewControl& vc, BuildingCreator& bc, AeroPhotoObjectList& mapList)
      : MouseEventHandler(vc, bc)
      , m_mapList(mapList)
    {}
    virtual ~MapViewEventHandler() {}

    // �}�E�X���ړ������Ƃ��ɁA�q��ʐ^�̃G���A�𒲂ׂ�
    virtual void onMouseMoved(MouseStatus& ms) {
      // �}�E�X�J�[�\���������Ă�����W���v�Z
      const RectS32& view = m_vc.getViewPort();
      const Matrix& vm = m_vc.getViewMatrix();
      Matrix um;
      Vector3D obj;
      if (!um.unProject(static_cast<float>(ms.x), static_cast<float>(view.height - ms.y - 1), 1.0f, vm, view, obj)) {
        return; // ���s
      }
      // ���_����}�E�X�J�[�\���̒����̕���������A�n�\�ʂƂ̌�_�����߂�
      Vector3Dd tg;
      tg.calcPointZonLine(m_vc.getEye(), obj, 0.0f);
      //TRACE("UnProject:(%d,%d) - (%.3f,%.3f,%.3f)\n", ms.x, ms.y, tg.x, tg.y, tg.z);

      // �Y������}�b�v��T���ăt�H�[�J�X���Z�b�g����
      if (m_mapList.findByMapPosition(tg, AppStatusManager::GetMapDirection(),  true) != AeroPhotoObjectList::InvalidId) {
        m_vc.setRedrawRequest();
      }
    }

    virtual void onLbtnReleased(MouseStatus& ms) {
      if (ms.click) {
        // �N���b�N�ŉ摜�ǂݍ���
        uint32_t id = m_mapList.getFocusedObjectId();
        if (id != AeroPhotoObjectList::InvalidId) {
          m_mapList.setCurrentAeroPhotoObject(id);
          m_vc.setRedrawRequest();
        }
      }
    }


    virtual void onLbtnDragging(MouseStatus& ms) {
      bool seeBottom = false;
      // L�{�^���h���b�O�Ŏ��_�܂��͕��s�ړ�
      // �����_���ʂł̉�ʂ����ς��̃T�C�Y
      float32_t vh = static_cast<float32_t>(tan(m_vc.getFovy() * Pai2Rad / 2.0) * m_vc.getEye().distance(m_vc.getTarget()) * 2.0);
      float32_t vw = static_cast<float32_t>(vh * m_vc.getViewAspect());
      vw *= -static_cast<float32_t>(ms.dX) / m_vc.getViewPort().width;
      vh *= static_cast<float32_t>(ms.dY) / m_vc.getViewPort().height;

      // �����̃x�N�g�����v�Z
      Vector3D ev(m_vc.camera().target - m_vc.camera().eye);
      if (fabs(ev.x) < 100.0f && fabs(ev.y) < 100.0f) {
        //�^�ォ�牺�������Ă���ꍇ
        ev = Vector3D(0.0f, 1.0f, 0.0f);
        if (abs(ms.dY) > abs(ms.dX)) vw = 0.0f;
        seeBottom = true;
      }
      ev.normalize();
      Vector3D sv(ev.y, -ev.x, -ev.z); // ev�ƒ��s����x�N�g��
      ev *= vh;
      sv *= vw;
      Vector3D mv(ev + sv);

      if (ms.btn.shiftKey || seeBottom) {
        // �V�t�g�������Ă���ꍇ�͕��s�ړ�
        if (ms.btn.ctrlKey) {
          // Shift+Ctrl��ViewControl��ObjectOffset��ݒ�
//          m_vc.addObjectOffset(Vector3D(vw, vh, 0.0f));
          if (seeBottom){ 
            m_vc.camera().eye.y = m_vc.camera().target.y - 1000.0;
          }
        }
        else {
          mv.z = 0;
          m_vc.camera().eye += mv;
          m_vc.camera().target += mv;
        }
        if (ms.btn.shiftKey) {
          // Shift��gldOrigin��ObjectOffset��ݒ�
          if (ms.btn.centerBtn) {
#if 0
            static float sx = 30.0f;
            static float sy = 30.0f;
            sx += vw / 100.0f;
            sy += vh / 100.0f;
            m_bc.getObjectList()->setScaleAll(Vector3D(sx,sy,10.0f));
#endif

          }
          else {
#if 0
            gldOrigin.x += vw;
            gldOrigin.y += vh;
            m_bc.getObjectList()->setOriginAll(gldOrigin);
            TRACE("Origin(%ld,%ld)\n", gldOrigin.x, gldOrigin.y);
#endif
          }
        }
      }
      else if (ms.btn.ctrlKey) {
        // CTRL�L�[�������Ă�Ƃ��́A���_�̂ݕ��s�ړ�
        mv.z = 0;
        m_vc.camera().eye += mv;
      }
      else {
        // ���Ă�������ɑ΂��Ē����_���ړ�
        if (seeBottom) {
          sv.y = vh;
          sv.z = 0.0;
          m_vc.camera().target += sv;

        }
        else {
          sv.z = vh;
          m_vc.camera().target += sv;

          // �����_��z��0�ɒ�������
          Vector3Dd& e = m_vc.camera().eye;
          Vector3Dd& t = m_vc.camera().target;
          // �����̕�����
          // (x-e.x)/(t.x-e.x)=(y-e.y)/(t.y-e.y)=(z-e.z)/(t.z-e.z)
          // z = 0������
          // (x-e.x)/(t.x-e.x) = (-e.z)/(t.z-e.z)
          // (x-e.x) = ((-e.z)/(t.z-e.z))/(t.x-e.x)
          // x = e.x + ((-e.z)/(t.z-e.z))/(t.x-e.x)
          m_vc.camera().target.x = e.x + ((-e.z)*(t.x-e.x))/(t.z-e.z);
          m_vc.camera().target.y = e.y + ((-e.z)*(t.y-e.y))/(t.z-e.z);
          m_vc.camera().target.z = 0.0;
        }

      }
      m_vc.setRedrawRequest();
    }

    virtual void onRbtnDragging(MouseStatus& ms) {
      float32_t ch = m_vc.getCameraHeight() - (ms.dY) * 5.0f;
      if (ch < 1.0f) ch = 1.0f;
      m_vc.setCameraHeight(ch);
      m_vc.camera().eye.z =  m_vc.getCameraHeight();
      m_vc.setRedrawRequest();
    }

    virtual void onMouseWheel(MouseStatus& ms)   {
      m_vc.addCameraFovy(ms.wheel / 480.0f);
      m_vc.setRedrawRequest();
    }
  };


} // namespace

#endif

