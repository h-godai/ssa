// ViewControl.hpp

// 3D-Viewの操作とカメラ位置の計算
// Platform非依存

#ifndef APP_VIEWCONTROL_H
#define APP_VIEWCONTROL_H

#ifndef STDAFX_H
#include <vector>
#endif
#include "Common/Common.h"
#include "Math/Vector.h"
#include "Math/Rect.h"
#include "Math/CameraPositionCalculator.h"


namespace app {
  using namespace ts;
  using std::vector;

  class ViewControl {
  public:

    enum CameraModeType {
      SEE_FREE,       // 自由なカメラワーク
      TOP_DOWN,       // 真上から見下ろす。上が北
      FIXED_TARGET,   // ターゲット固定モード
      CALC_CAMERAPOS, // カメラ位置計算モード
      EDIT_BUILDING,  // 建物編集モード
    };

    ViewControl() 
      : m_redrawRequest(false)
      , m_cameraHeight(1230.0)
      , m_cameraMode(TOP_DOWN)
      , m_zoomRatio(1)
      , m_draggingRect(0,0,0,0)
      , m_moveRectMode(0)
      , m_cameraPositionCalculator(m_camera, 2.0/3.0)
      , m_zNear(5.0)
      , m_zFar(5000.0)
    {
    }

    // 描画フレームの開始処理
    void beginFrame(IGraphicsContext* gc, IRenderer* gl, const RectS32& vp) {
      gc->setViewPort(vp);
      if (m_cameraMode == EDIT_BUILDING) {
        ::glMatrixMode(GL_PROJECTION);
        ::glLoadIdentity();
        double xmin, xmax, ymin, ymax;
        double aspect = static_cast<float32_t>(vp.width)/vp.height;
        ymax =  m_zNear * tan(static_cast<double>(getFovy() * 3.14159265358979 / 360.0));
        ymin = -ymax;
        xmin =  ymin * aspect;
        xmax =  ymax * aspect;
        ::glFrustum(xmin+m_viewOffset.x, xmax+m_viewOffset.x, ymin+m_viewOffset.y, ymax+m_viewOffset.y, m_zNear, m_zFar);
        ::glMatrixMode(GL_MODELVIEW);
        gl->lookAt(getEye(), getTarget(), getTop());
      }
      else {
        gl->perspective(getFovy(), static_cast<float32_t>(vp.width)/vp.height, m_zNear, m_zFar);
        gl->lookAt(getEye(), getTarget(), getTop());
      }
    }
    // 描画フレームの終了処理
    void endFrame(IGraphicsContext* , IRenderer* ) {
    }

    // 座標の配列からバウンダリボックスを計算する
    void addBoundary(const RectVector3Dd& data) {
      addBoundary(&data[0], 4);
    }

    void addBoundary(const Vector3Dd* data, size_t count) {
      while (count > 0) {
        m_boundary.set(*data);
        ++data;
        --count;
      }
    }

    void clearBoundary() {
      m_boundary.clear();
    }

    void setRole(float32_t role) {
      m_role = role;
      Vector3Dd eye(m_camera.target - m_camera.eye); // 視線ベクトル
      eye.normalize();
      Matrixd mat;
      mat.identity();
      mat.rotate(role*12.0, eye);
      if (sqrt(eye.x*eye.x + eye.y*eye.y) > fabs(eye.z)) { // 45度より上
        m_camera.top = Vector3Dd(0.0,0.0,1.0);
      }
      else {
        m_camera.top = Vector3Dd(0.0,1.0,0.0);
      }
      m_camera.top = mat.mult(m_camera.top);
      m_camera.top.normalize();
      setRedrawRequest();
    }

    void setCameraMode(CameraModeType pos) {
      m_cameraMode = pos;
      switch(pos) {
      case TOP_DOWN:
        {
          AeroPhotoObject::ptr mapObj(AppStatusManager::getCurrentAeroPhotoObject());
          if (mapObj.get() != 0) {
            m_camera.target = CameraPositionCalculator::getCenterPoint(mapObj->getCorners());
          }
          else {
            m_camera.target = m_boundary.center();
          }
          m_camera.eye = m_camera.target;
          m_camera.eye.z = m_cameraHeight;
          m_camera.top = Vector3Dd(0.0,1.0,0.0);
        }
        break;
      case EDIT_BUILDING:
        m_viewOffset = Vector2D(0.0f, 0.0f);
        break;
      case FIXED_TARGET:
        m_cameraHeight = static_cast<float32_t>(m_camera.eye.z);
        break;
      case CALC_CAMERAPOS:
        m_cameraHeight = static_cast<float32_t>(m_camera.eye.z);
        m_eyeStart = m_camera.eye;
        break;
      case SEE_FREE:
      default:
        m_cameraHeight = static_cast<float32_t>(m_camera.eye.z);
        if (m_camera.eye.x == m_camera.target.x && m_camera.eye.y == m_camera.target.y) {
          // 真下を向いていたら視点を修正
          seeDirection(AppStatusManager::GetMapDirection());
        }
        break;
      }
    }

    // カメラの向きを変える
    void seeDirection(MapDirection mapDir) {
      if (m_cameraMode == CALC_CAMERAPOS) return; // 自由モード以外は無視
      m_camera.top = Vector3Dd(0.0,0.0,1.0);
      m_camera.eye.x = m_camera.target.x;
      m_camera.eye.y = m_camera.target.y;
      switch (mapDir) {
      default:
      case MAPDIR_TOP:   m_camera.top = Vector3Dd(0.0, 1.0, 0.0); break;
      case MAPDIR_NORTH: m_camera.eye.y = m_camera.target.y - m_cameraHeight; break;
      case MAPDIR_SOUTH: m_camera.eye.y = m_camera.target.y + m_cameraHeight; break;
      case MAPDIR_WEST:  m_camera.eye.x = m_camera.target.x + m_cameraHeight; break;
      case MAPDIR_EAST:  m_camera.eye.x = m_camera.target.x - m_cameraHeight; break;
      }
    }

    // カメラの移動
    void moveToXy(Vector3Dd to) {
      m_camera.eye.x = to.x;
      m_camera.eye.y = to.y;
      m_camera.target.x = to.x;
      m_camera.target.y = to.y;
    }

    void addObjectOffset(const Vector3D& v) {
      m_objectOffset += v;
    }

    Vector3D& objectOffset() {
      return m_objectOffset;
    }

    // 1ピクセルあたりのサイズを返す
    Vector2D getSizePerPixel() const {
      return Vector2D(2.0f / m_viewPort.width, 2.0f / m_viewPort.height);
    }

    void addZoomRatio(int ratio) {
      m_zoomRatio += ratio;
    }

    // 画角の増減
    void addCameraFovy(float32_t d) {
      m_camera.fovy += d;
      if (m_camera.fovy > 180.0f) m_camera.fovy = 180.0f;
      if (m_camera.fovy < 0.01f) m_camera.fovy = 0.01f;
    }

    // setter / getter
    BoundaryBox<float64_t>& getBoundary() { return m_boundary; }

    void setRedrawRequest(bool redraw = true) { 
      if (m_cameraPositionCalculator.getCalcPhase() > 0) redraw = true;
      m_redrawRequest = redraw; 
    }
    int32_t getCalcPhase() {
      return m_cameraPositionCalculator.getCalcPhase();
    }


    bool getRedrawRequest() const { return m_redrawRequest; }


    const Vector3Dd& getTop() const { return m_camera.top; }
    double getViewAspect() const { return static_cast<double>(m_viewPort.width)/m_viewPort.height; }
    double getFovy() const { return m_camera.fovy; }

    void setViewPort(const RectS32& viewPort) { m_viewPort = viewPort; }
    const RectS32& getViewPort() const { return m_viewPort; }

    void setEye(const Vector3Dd& eye) { m_camera.eye = eye; }
    const Vector3Dd& getEye() const { return m_camera.eye; }

    void setTarget(const Vector3Dd& tg) { m_camera.target = tg; }
    const Vector3Dd& getTarget() const { return m_camera.target; }

    void setCameraPos(const CameraPosition& camera) { m_camera = camera; }
    const CameraPosition& getCamera() const { return m_camera; }
    CameraPosition& camera() { return m_camera; }

    CameraModeType getCameraMode() const { return m_cameraMode; }

    void setCameraHeight(float32_t ch) { m_cameraHeight = ch; }
    float32_t getCameraHeight() const { return m_cameraHeight; }

    void setZoomRatio(int32_t ratio) { m_zoomRatio = ratio; }
    int32_t getZoomRatio() const { return m_zoomRatio; }

    void setDraggingRect(const RectS32& rect) { m_draggingRect = rect; }
    const RectS32& getDraggingRect() const { return m_draggingRect; }

    void setRightTopViewPort(const RectS32& rect) { m_viewPortRT = rect; }
    const RectS32& getRightTopViewPort() const { return m_viewPortRT; }

    void setRightBottomViewPort(const RectS32& rect) { m_viewPortRB = rect; }
    const RectS32& getRightBottomViewPort() const { return m_viewPortRB; }

    void setMoveRectMode(int mode) { m_moveRectMode = mode; }
    int getMoveRectMode() const { return m_moveRectMode; }

    CameraPositionCalculator& getCameraPositionCalculator() {
      return m_cameraPositionCalculator;
    }

    const Matrix& getViewMatrix() const { return m_viewMatrix; }
    void setViewMatrix(const Matrix& m) { m_viewMatrix = m; }

    void setViewOffset(const Vector2D& v) { m_viewOffset = v; }
    void addViewOffset(const Vector2D& v) { m_viewOffset += v; }

    Vector3Dd m_eyeStart;
    float64_t getZNear() const { return m_zNear; }
    float64_t getZFar() const { return m_zFar; }
  private:

    RectS32 m_viewPort;
    RectS32 m_viewPortRT; // 右上のView
    RectS32 m_viewPortRB; // 右下のView
    BoundaryBox<float64_t> m_boundary;
    bool m_redrawRequest;
    int32_t m_zoomRatio;
    RectS32 m_draggingRect;
    int m_moveRectMode;
    Matrix m_viewMatrix;

    // camea param
    CameraPosition m_camera;
    float32_t m_cameraHeight;
    CameraModeType m_cameraMode;
    float32_t m_role;

    Vector3D m_objectOffset;
    Vector2D m_viewOffset;
    float64_t m_zNear;
    float64_t m_zFar;


    CameraPositionCalculator m_cameraPositionCalculator;
  };






} // namespace

#endif
