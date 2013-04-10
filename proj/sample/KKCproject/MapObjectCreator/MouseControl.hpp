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
    bool click; // ボタンが同じ位置で押して離された場合にtrue
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
      BTN_NOACTION,  // ボタンが離されたまま
      BTN_PUSHED,    // ボタンが押された
      BTN_RELEASED,  // ボタンが離された
      BTN_DRAGGING,  // ボタンが押されたまま移動
    };
    struct Event {
      EventType lBtn; // 左ボタン
      EventType rBtn; // 右ボタン
      bool mouseMoved; // マウスが移動した
      bool mouseWheel; // マウスホイールが回された
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
    int lPushX; // Lボタンが押された位置
    int lPushY;

    // 現在のステータスとnewStatusでイベントの種別を計算する
    Event getEvent(const MouseStatus& newStatus) {
      MouseStatus change(status.data ^ newStatus.data);
      int dX = newStatus.x - status.x;
      int dY = newStatus.y - status.y;
      status = newStatus;
      status.dX = dX;
      status.dY = dY;
      Event ev;
      if (status.dX != 0 || status.dY != 0)  ev.mouseMoved = true;
      if (change.btn.leftBtn) {  // Lbtnが変化
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
      else {  // 変化なし
        if (ev.mouseMoved && status.btn.leftBtn != 0) ev.lBtn = BTN_DRAGGING;   // 押されたまま移動
      }
      if (change.btn.rightBtn) {  // Rbtnが変化
        ev.rBtn = status.btn.rightBtn ? BTN_PUSHED : BTN_RELEASED;
      }
      else {  // 変化なし
        if (ev.mouseMoved && status.btn.rightBtn != 0) ev.rBtn = BTN_DRAGGING;   // 押されたまま移動
      }
      if (status.wheel != 0) ev.mouseWheel = true;
      return ev;
    }

    // イベントの種別によってイベントハンドラのメソッドを起動する
    void eventDispatch(const MouseStatus& newStatus) {
      if (eventHandler.get() == 0) return;
      Event ev = getEvent(newStatus);
      MouseStatus s = status;
      switch(ev.lBtn) {
      case BTN_NOACTION:  eventHandler->onLbtnIdle(status);     break; // ボタンが離されたまま
      case BTN_PUSHED:    eventHandler->onLbtnPushed(status);   break; // ボタンが押された
      case BTN_RELEASED:  eventHandler->onLbtnReleased(status); break; // ボタンが離された
      case BTN_DRAGGING:  eventHandler->onLbtnDragging(status); break; // ボタンが押されたまま移動
      default: throw std::runtime_error("Invalid MouseEvent type");
      }
      status= s;
      switch(ev.rBtn) {
      case BTN_NOACTION:  eventHandler->onRbtnIdle(status);     break; // ボタンが離されたまま
      case BTN_PUSHED:    eventHandler->onRbtnPushed(status);   break; // ボタンが押された
      case BTN_RELEASED:  eventHandler->onRbtnReleased(status); break; // ボタンが離された
      case BTN_DRAGGING:  eventHandler->onRbtnDragging(status); break; // ボタンが押されたまま移動
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
