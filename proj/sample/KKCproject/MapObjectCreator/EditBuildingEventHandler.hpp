// EditBuildingEventHandler.hpp
//
// �����ҏW�}�E�X�C�x���g�n���h��
//

#ifndef EDITBUILDINGEVENTHANDLER_HPP
#define EDITBUILDINGEVENTHANDLER_HPP

#include "Util/util.hpp"

#include "MouseEventHandler.hpp"
#include "UiEventHandler.hpp"

namespace app {
  using namespace ts;
  using ts::logic::tribool;
  using ts::logic::indeterminate;

  // ���z���ҏW���[�h ���C���r���[
  class EditBuildingMouseEventHandlerMain : public MouseEventHandler {
  public:
    EditBuildingMouseEventHandlerMain(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
      , m_hitting(false)   // ���_��Ƀ}�E�X������A�{�^����������Ă��Ȃ�
      , m_resizing(indeterminate)  // ���T�C�Y��
    {}
    virtual ~EditBuildingMouseEventHandlerMain() {}

    virtual void onMouseMoved(MouseStatus& ms) {
      float32_t x = (2.0f * ms.x/m_vc.getViewPort().width) - 1.0f;
      float32_t y = -1.0f * ((2.0f * ms.y/m_vc.getViewPort().height) - 1.0f);
      m_hitting = false;
      if (ms.btn.leftBtn == 0 && ms.btn.rightBtn == 0) {
        if (m_bc.hitCheckVertex2D(Vector2D(x, y), m_vc.getSizePerPixel())) {
          m_vc.setRedrawRequest();
          m_hitting = true;
        }
      }
    }

    virtual void onLbtnPushed(MouseStatus& /*ms*/) {
      //float32_t x = (2.0f * ms.x/m_vc.getViewPort().width) - 1.0f;
      //float32_t y = -1.0f * ((2.0f * ms.y/m_vc.getViewPort().height) - 1.0f);
      //if (!m_hitting && ms.btn.rightBtn == 0) {
        //if (!m_bc.isSingleObjectMode()) {
        //  m_bc.setCurrentObjectFor2D(x, y);
        //  m_vc.setRedrawRequest();
        //}
      //}
      m_resizing = indeterminate;
    }
    virtual void onLbtnReleased(MouseStatus& ms) {
      float32_t x = (2.0f * ms.x/m_vc.getViewPort().width) - 1.0f;
      float32_t y = -1.0f * ((2.0f * ms.y/m_vc.getViewPort().height) - 1.0f);
      if (!m_hitting && ms.btn.rightBtn == 0) {
        if (ms.click) {
          m_bc.setCurrentObjectFor2D(Vector2D(x, y));
          m_vc.setRedrawRequest();
        }
      }
      m_resizing = indeterminate;
    }

    virtual void onLbtnDragging(MouseStatus& ms) {
      Vector2D xy((2.0f * ms.x/m_vc.getViewPort().width) - 1.0f,
                   -1.0f * ((2.0f * ms.y/m_vc.getViewPort().height) - 1.0f));
      Vector3D dragSize(ms.dX/3.0f, -ms.dY/3.0f, 0.0f);
      BuildingObject::ptr bldg(m_bc.getCurrentObject());
      if (m_resizing != false && bldg.get() != 0 
        && (m_resizing == true || m_bc.hitCheckVertex2D(xy, m_vc.getSizePerPixel()) || bldg->getObject()->isPointInRect2D(xy))) {
        // �J�����g�r��������A���T�C�Y�������_���o�E���_���{�b�N�X�ɃJ�[�\��������ꍇ
        m_resizing = true;
        if (ms.btn.ctrlKey) {
          // CTRL�L�[��������Ă���
          if (ms.btn.shiftKey || m_bc.isCutBuildingMode()) {
            // SHIFT+CTRL�Ńo�E�_���{�b�N�X�̉�]
            bldg->setBbDirection(ms.dX*0.5f);
          }
          else {
            bldg->setDirection(ms.dX*0.5f); // �r���̌���
          }
        }
        else if (ms.btn.shiftKey) {
            bldg->addScale(Vector3Dd(ms.dX/3.0f, -ms.dY/3.0f, 0.0f)); // �r���̑傫��
        }
        else {
          if (m_bc.isCutBuildingMode()) {
            // �����������[�h
            m_bc.moveCutSurface(dragSize);
          }
          else {
            if (m_bc.isHitVertex()) {
              // ���_���ړ�
              m_bc.moveCurrentVertex(dragSize);
            }
            else {
              // �����̈ʒu���ړ�
              m_bc.moveCurrentBuilding(dragSize);
            }
          }
        }
      }
      else {
        // �r���[�̈ړ�
        m_vc.addViewOffset(Vector2D(-dragSize.x/200.0f, -dragSize.y/200.0f));
        m_resizing = false;
      }
      m_vc.setRedrawRequest();
    }

    virtual void onRbtnDragging(MouseStatus& ms) {
      Vector3D dragSize(ms.dX/100.0f, -ms.dY/100.0f, 0.0f);
      BuildingObject::ptr bldg(m_bc.getCurrentObject());
      if (bldg.get() != 0) {
        if (m_bc.isCutBuildingMode()) {
          // �o�E�_���{�b�N�X�̉�]
          bldg->setBbDirection(ms.dX*0.5f);
        }
        else {
          bldg->addScale(Vector3Dd(0.0f, 0.0f, dragSize.y)); // �r���̍���
        }
      }
      m_vc.setRedrawRequest();
    }


    virtual void onMouseWheel(MouseStatus& ms)   {
      m_vc.camera().fovy += ms.wheel / 480.0f;
      m_vc.setRedrawRequest();
    }

  private:
    bool m_hitting;  // ���_��Ƀ}�E�X������A�{�^����������Ă��Ȃ�
    tribool m_resizing; // ���T�C�Y��
  };

  // �E��̃g�b�v�r���[
  class EditBuildingMouseEventHandlerRT : public MouseEventHandler {
  public:
    EditBuildingMouseEventHandlerRT(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
    {}
    virtual ~EditBuildingMouseEventHandlerRT() {}
    virtual void onLbtnDragging(MouseStatus& ms) {
      Vector3D dragSize(ms.dX/3.0f, -ms.dY/3.0f, 0.0f);
      BuildingObject::ptr bldg(m_bc.getCurrentObject());
      if (bldg.get() != 0) {
        bldg->setDirection(ms.dX*0.3f); // �r���̌���
      }
      m_vc.setRedrawRequest();
    }

    virtual void onRbtnDragging(MouseStatus& ms) {
      Vector3D dragSize(ms.dX/3.0f, -ms.dY/3.0f, 0.0f);
      BuildingObject::ptr bldg(m_bc.getCurrentObject());
      if (bldg.get() != 0) {
        bldg->addScale(Vector3Dd(0.0f, 0.0f, -ms.dY/3.0f)); // �r���̍���
      }
      m_vc.setRedrawRequest();
    }
  };

  // �E���̃e�N�X�`���[�r���[
  class EditBuildingMouseEventHandlerRB : public MouseEventHandler {
  public:
    EditBuildingMouseEventHandlerRB(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
    {}
    virtual ~EditBuildingMouseEventHandlerRB() {}

    virtual void onLbtnPushed(MouseStatus& ms) {
      BuildingObject::ptr bl(m_bc.getCurrentObject());
      if (bl.get() == 0) return;
      int32_t w = m_vc.getRightBottomViewPort().width;
      int32_t h = m_vc.getRightBottomViewPort().height;
      float32_t x = static_cast<float32_t>(ms.x) / w;
      float32_t y = static_cast<float32_t>(ms.y) / h;
      bl->setCurrentTextureSurface(Vector2D(x,y));
      m_vc.setRedrawRequest();
    }


  };

  // �����ҏW���[�h
  class EditBuildingEventHandler : public EditModeMouseEventHandler, public IUiEventHandler {
  public:
    typedef shared_ptr<EditBuildingEventHandler> ptr;
    EditBuildingEventHandler(ViewControl& vc, BuildingCreator& bc)
      : EditModeMouseEventHandler(vc, bc)
      , m_main(vc, bc)
      , m_RT(vc, bc)
      , m_RB(vc, bc)
    {
      EditModeMouseEventHandler::initialize(m_main, m_RT, m_RB);
    }
    virtual ~EditBuildingEventHandler() {}
    virtual void onButtonCopy() {}
    virtual void onButtonPaste() {}

  private:
    EditBuildingMouseEventHandlerMain m_main;
    EditBuildingMouseEventHandlerRT   m_RT;
    EditBuildingMouseEventHandlerRB   m_RB;
  };



} // namespace

#endif

