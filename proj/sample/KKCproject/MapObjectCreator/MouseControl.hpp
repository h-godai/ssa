// MouseControl.hpp
#include "Common/Common.h"
#include <stdexcept>

#ifndef MOUSECONTROL_HPP
#define MOUSECONTROL_HPP

namespace app {
  using boost::shared_ptr;

  struct MouseStatus {
    union {
      uint32_t data;
      struct BtnFlags {
        int leftBtn:1;
        int rightBtn:1;
        int centerBtn:1;
        int ctrlKey:1;
        int shiftKey:1;
      } btn;
    };
    int x;
    int y;
    int dX;
    int dY;
    int wheel;
    bool click; // �{�^���������ʒu�ŉ����ė����ꂽ�ꍇ��true
    MouseStatus(int s = 0)
      : data(s), x(0), y(0), dX(0), dY(0), wheel(0) {
    }
  };

  class IMouseEventHandler {
  public:
    virtual ~IMouseEventHandler() {}
    virtual void onLbtnIdle(MouseStatus& ) {}
    virtual void onLbtnPushed(MouseStatus& ) {}
    virtual void onLbtnReleased(MouseStatus& ) {}
    virtual void onLbtnDragging(MouseStatus& ) {}
    virtual void onRbtnIdle(MouseStatus& ) {}
    virtual void onRbtnPushed(MouseStatus& ) {}
    virtual void onRbtnReleased(MouseStatus& ) {}
    virtual void onRbtnDragging(MouseStatus& ) {}
    virtual void onMouseMoved(MouseStatus& ) {}
    virtual void onMouseWheel(MouseStatus& ) {}
  };

  struct MouseEvent {
    enum EventType {
      BTN_NOACTION,  // �{�^���������ꂽ�܂�
      BTN_PUSHED,    // �{�^���������ꂽ
      BTN_RELEASED,  // �{�^���������ꂽ
      BTN_DRAGGING,  // �{�^���������ꂽ�܂܈ړ�
    };
    struct Event {
      EventType lBtn; // ���{�^��
      EventType rBtn; // �E�{�^��
      bool mouseMoved; // �}�E�X���ړ�����
      bool mouseWheel; // �}�E�X�z�C�[�����񂳂ꂽ
      Event()
        : lBtn(BTN_NOACTION)
        , rBtn(BTN_NOACTION)
        , mouseMoved(false)
        , mouseWheel(false)
      {
      }
    };
    MouseStatus status;
    shared_ptr<IMouseEventHandler> eventHandler;
    int lPushX; // L�{�^���������ꂽ�ʒu
    int lPushY;

    // ���݂̃X�e�[�^�X��newStatus�ŃC�x���g�̎�ʂ��v�Z����
    Event getEvent(const MouseStatus& newStatus) {
      MouseStatus change(status.data ^ newStatus.data);
      int dX = newStatus.x - status.x;
      int dY = newStatus.y - status.y;
      status = newStatus;
      status.dX = dX;
      status.dY = dY;
      Event ev;
      if (status.dX != 0 || status.dY != 0)  ev.mouseMoved = true;
      if (change.btn.leftBtn) {  // Lbtn���ω�
        if ((status.btn.leftBtn) != 0) {
            ev.lBtn = BTN_PUSHED;
            lPushX = newStatus.x;
            lPushY = newStatus.y;
        }
        else {
          ev.lBtn = BTN_RELEASED;
          status.click = (lPushX == newStatus.x && lPushY == newStatus.y);
        }
      }
      else {  // �ω��Ȃ�
        if (ev.mouseMoved && status.btn.leftBtn != 0) ev.lBtn = BTN_DRAGGING;   // �����ꂽ�܂܈ړ�
      }
      if (change.btn.rightBtn) {  // Rbtn���ω�
        ev.rBtn = status.btn.rightBtn ? BTN_PUSHED : BTN_RELEASED;
      }
      else {  // �ω��Ȃ�
        if (ev.mouseMoved && status.btn.rightBtn != 0) ev.rBtn = BTN_DRAGGING;   // �����ꂽ�܂܈ړ�
      }
      if (status.wheel != 0) ev.mouseWheel = true;
      return ev;
    }

    // �C�x���g�̎�ʂɂ���ăC�x���g�n���h���̃��\�b�h���N������
    void eventDispatch(const MouseStatus& newStatus) {
      if (eventHandler.get() == 0) return;
      Event ev = getEvent(newStatus);
      MouseStatus s = status;
      switch(ev.lBtn) {
      case BTN_NOACTION:  eventHandler->onLbtnIdle(status);     break; // �{�^���������ꂽ�܂�
      case BTN_PUSHED:    eventHandler->onLbtnPushed(status);   break; // �{�^���������ꂽ
      case BTN_RELEASED:  eventHandler->onLbtnReleased(status); break; // �{�^���������ꂽ
      case BTN_DRAGGING:  eventHandler->onLbtnDragging(status); break; // �{�^���������ꂽ�܂܈ړ�
      default: throw std::runtime_error("Invalid MouseEvent type");
      }
      status= s;
      switch(ev.rBtn) {
      case BTN_NOACTION:  eventHandler->onRbtnIdle(status);     break; // �{�^���������ꂽ�܂�
      case BTN_PUSHED:    eventHandler->onRbtnPushed(status);   break; // �{�^���������ꂽ
      case BTN_RELEASED:  eventHandler->onRbtnReleased(status); break; // �{�^���������ꂽ
      case BTN_DRAGGING:  eventHandler->onRbtnDragging(status); break; // �{�^���������ꂽ�܂܈ړ�
      default: throw std::runtime_error("Invalid MouseEvent type");
      }
      status= s;

      if (ev.mouseMoved) {
        eventHandler->onMouseMoved(status);
      }
      if (ev.mouseWheel) {
        eventHandler->onMouseWheel(status);
      }
      status= s;
    }
  };

} // namespace 

#endif
