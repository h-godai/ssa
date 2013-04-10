// MapViewEventHandler.hpp
//
// マップビューモードイベントハンドラ
//

#ifndef MAPVIEWEVENTHANDLER_HPP
#define MAPVIEWEVENTHANDLER_HPP

#include "MouseEventHandler.hpp"
#include "AeroPhotoObjectList.hpp"

namespace app {
  extern Vector3D gldOrigin;

  // ビューモード
  class MapViewEventHandler : public MouseEventHandler {
    AeroPhotoObjectList& m_mapList;
  public:
    typedef shared_ptr<MapViewEventHandler> ptr;
    MapViewEventHandler(ViewControl& vc, BuildingCreator& bc, AeroPhotoObjectList& mapList)
      : MouseEventHandler(vc, bc)
      , m_mapList(mapList)
    {}
    virtual ~MapViewEventHandler() {}

    // マウスが移動したときに、航空写真のエリアを調べる
    virtual void onMouseMoved(MouseStatus& ms) {
      // マウスカーソルが示している座標を計算
      const RectS32& view = m_vc.getViewPort();
      const Matrix& vm = m_vc.getViewMatrix();
      Matrix um;
      Vector3D obj;
      if (!um.unProject(static_cast<float>(ms.x), static_cast<float>(view.height - ms.y - 1), 1.0f, vm, view, obj)) {
        return; // 失敗
      }
      // 視点からマウスカーソルの直線の方程式から、地表面との交点を求める
      Vector3Dd tg;
      tg.calcPointZonLine(m_vc.getEye(), obj, 0.0f);
      //TRACE("UnProject:(%d,%d) - (%.3f,%.3f,%.3f)\n", ms.x, ms.y, tg.x, tg.y, tg.z);

      // 該当するマップを探してフォーカスをセットする
      if (m_mapList.findByMapPosition(tg, AppStatusManager::GetMapDirection(),  true) != AeroPhotoObjectList::InvalidId) {
        m_vc.setRedrawRequest();
      }
    }

    virtual void onLbtnReleased(MouseStatus& ms) {
      if (ms.click) {
        // クリックで画像読み込み
        uint32_t id = m_mapList.getFocusedObjectId();
        if (id != AeroPhotoObjectList::InvalidId) {
          m_mapList.setCurrentAeroPhotoObject(id);
          m_vc.setRedrawRequest();
        }
      }
    }


    virtual void onLbtnDragging(MouseStatus& ms) {
      bool seeBottom = false;
      // Lボタンドラッグで視点または並行移動
      // 注視点平面での画面いっぱいのサイズ
      float32_t vh = static_cast<float32_t>(tan(m_vc.getFovy() * Pai2Rad / 2.0) * m_vc.getEye().distance(m_vc.getTarget()) * 2.0);
      float32_t vw = static_cast<float32_t>(vh * m_vc.getViewAspect());
      vw *= -static_cast<float32_t>(ms.dX) / m_vc.getViewPort().width;
      vh *= static_cast<float32_t>(ms.dY) / m_vc.getViewPort().height;

      // 視線のベクトルを計算
      Vector3D ev(m_vc.camera().target - m_vc.camera().eye);
      if (fabs(ev.x) < 100.0f && fabs(ev.y) < 100.0f) {
        //真上から下を向いている場合
        ev = Vector3D(0.0f, 1.0f, 0.0f);
        if (abs(ms.dY) > abs(ms.dX)) vw = 0.0f;
        seeBottom = true;
      }
      ev.normalize();
      Vector3D sv(ev.y, -ev.x, -ev.z); // evと直行するベクトル
      ev *= vh;
      sv *= vw;
      Vector3D mv(ev + sv);

      if (ms.btn.shiftKey || seeBottom) {
        // シフトを押している場合は平行移動
        if (ms.btn.ctrlKey) {
          // Shift+CtrlでViewControlのObjectOffsetを設定
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
          // ShiftでgldOriginのObjectOffsetを設定
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
        // CTRLキーを押してるときは、視点のみ平行移動
        mv.z = 0;
        m_vc.camera().eye += mv;
      }
      else {
        // 見ている方向に対して注視点を移動
        if (seeBottom) {
          sv.y = vh;
          sv.z = 0.0;
          m_vc.camera().target += sv;

        }
        else {
          sv.z = vh;
          m_vc.camera().target += sv;

          // 注視点のzを0に調整する
          Vector3Dd& e = m_vc.camera().eye;
          Vector3Dd& t = m_vc.camera().target;
          // 直線の方程式
          // (x-e.x)/(t.x-e.x)=(y-e.y)/(t.y-e.y)=(z-e.z)/(t.z-e.z)
          // z = 0だから
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

