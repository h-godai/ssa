// MouseEventHandler.hpp
//
// マウスイベントハンドラ
//

#ifndef MOUSEEVENTHANDLER_HPP
#define MOUSEEVENTHANDLER_HPP


#include "RenderMain.hpp"
#include "ViewControl.hpp"
#include "BuildingCreator.hpp"
#include "MouseControl.hpp"

namespace app {

  class MouseEventHandler : public IMouseEventHandler {
  public:
    MouseEventHandler(ViewControl& vc, BuildingCreator& bc)
      : m_vc(vc)
      , m_bc(bc)
    {
    }
    virtual ~MouseEventHandler() {}
    // マウスカーソル部分に建物のオブジェクトがあるか調べる

  protected:
    ViewControl& m_vc;
    BuildingCreator& m_bc;
  };


  // 3画面編集モード用のマウスイベントはんどら
  // それぞれのビューにイベントを分ける
  class EditModeMouseEventHandler : public MouseEventHandler {
  public:
    EditModeMouseEventHandler(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
    {
    }
    virtual ~EditModeMouseEventHandler() {}
    void initialize(IMouseEventHandler& main, IMouseEventHandler& rt, IMouseEventHandler& rb) {
      m_eh[0] = &main;
      m_eh[1] = &rt;
      m_eh[2] = &rb;
    }

    enum ViewType {
      MAIN_VIEW = 0,
      RIGHT_TOP_VIEW,
      RIGHT_BOTTOM_VIEW
    };
    
    ViewType selectView(MouseStatus& ms) {
      int y = m_vc.getViewPort().height - ms.y - 1; // Y軸の反転
      if (m_vc.getRightTopViewPort().isInside(ms.x, y)) {
        ms.x -= m_vc.getRightTopViewPort().left;
        return RIGHT_TOP_VIEW;
      }
      if (m_vc.getRightBottomViewPort().isInside(ms.x, y)) {
        ms.x -= m_vc.getRightBottomViewPort().left;
        ms.y -= m_vc.getViewPort().height - m_vc.getRightBottomViewPort().height;
        return RIGHT_BOTTOM_VIEW;
      }
      return MAIN_VIEW;
    }

    virtual void onLbtnIdle(MouseStatus& ms)     { m_eh[selectView(ms)]->onLbtnIdle(ms); }
    virtual void onLbtnPushed(MouseStatus& ms)   { m_eh[selectView(ms)]->onLbtnPushed(ms); }
    virtual void onLbtnReleased(MouseStatus& ms) { m_eh[selectView(ms)]->onLbtnReleased(ms); }
    virtual void onLbtnDragging(MouseStatus& ms) { m_eh[selectView(ms)]->onLbtnDragging(ms); }
    virtual void onRbtnIdle(MouseStatus& ms)     { m_eh[selectView(ms)]->onRbtnIdle(ms); }
    virtual void onRbtnPushed(MouseStatus& ms)   { m_eh[selectView(ms)]->onRbtnPushed(ms); }
    virtual void onRbtnReleased(MouseStatus& ms) { m_eh[selectView(ms)]->onRbtnReleased(ms); }
    virtual void onRbtnDragging(MouseStatus& ms) { m_eh[selectView(ms)]->onRbtnDragging(ms); }
    virtual void onMouseMoved(MouseStatus& ms)   { m_eh[selectView(ms)]->onMouseMoved(ms);  }
    virtual void onMouseWheel(MouseStatus& ms)   { m_eh[selectView(ms)]->onMouseWheel(ms);  }

  private:
    IMouseEventHandler* m_eh[3];
  };



} // namespace

#endif

