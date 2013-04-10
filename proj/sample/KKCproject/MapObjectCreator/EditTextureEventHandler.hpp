// EditTextureEventHandler.hpp
//
// テクスチャ編集モードマウスイベントハンドラ
//

#ifndef EDITTEXTUREEVENTHANDLER_HPP
#define EDITTEXTUREEVENTHANDLER_HPP


#include "MouseEventHandler.hpp"
#include "UiEventHandler.hpp"

namespace app {

  
  // テクスチャ編集モードメイン画面
  class EditTextureMouseEventHandlerMain : public MouseEventHandler {
  public:
    EditTextureMouseEventHandlerMain(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
      , m_rect(0,0,0,0)
      , m_moveRect(false)
    {}
    virtual ~EditTextureMouseEventHandlerMain() {}

    virtual void onMouseWheel(MouseStatus& ms)   {
      int32_t ratio = m_vc.getZoomRatio() + (ms.wheel > 0 ? 1 : -1 );
      if (ratio >= 0 && ratio <= 4) {
        m_vc.setZoomRatio(ratio);
      }
      m_vc.setRedrawRequest();
    }
    
    virtual void onLbtnPushed(MouseStatus& ms) {
      m_dragX = ms.x;
      m_dragY = ms.y;
      if (!m_rect.isEmpty() && m_rect.isInside(ms.x, ms.y)) {
        m_moveRect = true; // 矩形移動モード
        m_vc.setMoveRectMode(1);
      }
      else {
        // 選択範囲以外でクリックされた
        m_moveRect = false;
        m_vc.setMoveRectMode(0);
        m_rect.clear();
        m_vc.setDraggingRect(m_rect);
        m_vc.setRedrawRequest();
      }
    }
    virtual void onLbtnReleased(MouseStatus& ) {
      if (m_moveRect) {
        m_vc.setMoveRectMode(2);
        m_vc.setRedrawRequest();
      }
    }
    virtual void onLbtnDragging(MouseStatus& ms) {
      if (m_moveRect) {
        m_rect.left += ms.dX;
        m_rect.top += ms.dY;
      }
      else {
        // 矩形作成時
        m_rect = RectS32(m_dragX, m_dragY, ms.x - m_dragX, ms.y - m_dragY);
      }
      m_vc.setDraggingRect(m_rect);
      m_vc.setRedrawRequest();
    }

    // コピーボタンが押された!
    void onButtonCopy() {
    }
    void onButtonPaste() {
      if (m_moveRect && !m_rect.isEmpty()) {
        m_bc.pasteTextureImage();
        m_moveRect = false;
        m_rect.clear();
        m_vc.setDraggingRect(m_rect);
        m_vc.setMoveRectMode(0);
        m_vc.setRedrawRequest();
      }
    }

  private:
    int m_dragX;
    int m_dragY;
    RectS32 m_rect;
    bool m_moveRect;
  };

  // テクスチャ編集モード右上画面
  class EditTextureMouseEventHandlerRT : public MouseEventHandler {
  public:
    EditTextureMouseEventHandlerRT(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
    {}
    virtual ~EditTextureMouseEventHandlerRT() {}
  };
  // テクスチャ編集モード右下画面
  class EditTextureMouseEventHandlerRB : public MouseEventHandler {
  public:
    EditTextureMouseEventHandlerRB(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
    {}
    virtual ~EditTextureMouseEventHandlerRB() {}
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

  // テクスチャ編集モード
  class EditTextureEventHandler : public EditModeMouseEventHandler, public IUiEventHandler {
  public:
    typedef shared_ptr<EditTextureEventHandler> ptr;
    EditTextureEventHandler(ViewControl& vc, BuildingCreator& bc)
      : EditModeMouseEventHandler(vc, bc)
      , m_main(vc, bc)
      , m_RT(vc, bc)
      , m_RB(vc, bc)
    {
      EditModeMouseEventHandler::initialize(m_main, m_RT, m_RB);
    }
    virtual ~EditTextureEventHandler() {}

    virtual void onButtonCopy() { m_main.onButtonCopy(); }
    virtual void onButtonPaste() { m_main.onButtonPaste();}

  private:
    EditTextureMouseEventHandlerMain m_main;
    EditTextureMouseEventHandlerRT   m_RT;
    EditTextureMouseEventHandlerRB   m_RB;
  };


} // namespace

#endif

