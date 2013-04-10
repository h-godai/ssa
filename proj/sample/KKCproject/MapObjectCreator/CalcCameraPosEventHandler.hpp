// CalcCameraPosEventHandler.hpp
//
// マウスイベントハンドラ
//

#ifndef CALCCAMERAPOSEVENTHANDER_HPP
#define CALCCAMERAPOSEVENTHANDER_HPP


#include "MouseEventHandler.hpp"

namespace app {

    // カメラ位置計算モード
  class CalcCameraPosEventHandler : public MouseEventHandler {
  public:
    typedef shared_ptr<CalcCameraPosEventHandler> ptr;
    CalcCameraPosEventHandler(ViewControl& vc, BuildingCreator& bc)
      : MouseEventHandler(vc, bc)
    {}
    virtual ~CalcCameraPosEventHandler() {}
    virtual void onLbtnDragging(MouseStatus& ms) {
      // Lボタンドラッグで視点または並行移動
      // 注視点平面での画面いっぱいのサイズ
      float32_t vh = static_cast<float32_t>(tan(m_vc.getFovy() * Pai2Rad) * m_vc.getEye().distance(m_vc.getTarget()));
      float32_t vw = static_cast<float32_t>(vh * m_vc.getViewAspect());
      vw *= -static_cast<float32_t>(ms.dX) / m_vc.getViewPort().width;
      vh *= static_cast<float32_t>(ms.dY) / m_vc.getViewPort().height;
      m_vc.camera().eye.x += vw;
      m_vc.camera().eye.y += vh;
      m_vc.setRedrawRequest();
    }

    virtual void onRbtnDragging(MouseStatus& ms) {
      float32_t ch = m_vc.getCameraHeight() - (ms.dY) * 5.0f;
      if (ch < 1.0f) ch = 1.0f;
      m_vc.setCameraHeight(ch);
      float64_t dis = (ms.dX)*1.0;
      Vector3Dd v(m_vc.camera().target - m_vc.camera().eye);
      v.normalize();
      m_vc.camera().eye.z =  m_vc.getCameraHeight();

      // v1-v2の直線の方程式 ax+by+c = 0;
      float64_t a = m_vc.m_eyeStart.y - m_vc.camera().target.y;
      float64_t b = m_vc.camera().target.x - m_vc.m_eyeStart.x;
      float64_t c = -a * m_vc.camera().target.x - b * m_vc.camera().target.y;
      m_vc.camera().eye.y += dis;
      m_vc.camera().eye.x = (-b*m_vc.camera().eye.y - c)/a;
      if (m_vc.camera().eye.z < 1.0) m_vc.camera().eye.z = 1.0;

      m_vc.setRedrawRequest();
    }

    virtual void onMouseWheel(MouseStatus& ms)   {
      m_vc.addCameraFovy(ms.wheel / 480.0f);
      m_vc.setRedrawRequest();
    }
  };

} // namespace

#endif

