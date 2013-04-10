// RenderMain.hpp
//
// 描画処理のメイン部分
// Platform非依存

#ifndef RENDERMAIN_HPP
#define RENDERMAIN_HPP

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "Graphics/IGraphicsContext.h"
#include "Graphics/GraphicsObject.h"
#include "PlatformDependent/Thread/Thread.hpp"
#include "AppStatusManager.hpp"
#include "ViewControl.hpp"
#include "BuildingCreator.hpp"

namespace app {

  using namespace ts;
  using boost::shared_ptr;


  class RenderMain : private IAppStatusEventHandler {
  public:
    RenderMain(BuildingCreator& bc)
      : m_bc(bc)
      , m_world(new Group)
      , m_north(new Group)
      , m_south(new Group)
      , m_west(new Group)
      , m_east(new Group)
      , m_top(new Group)
      , m_grid(new Group)
      , m_ground(new Group)
      , m_show2D(true)
      , m_aspect2D(0.0f)
      , m_editView(false)
      , m_groundMap(true)
      , m_enableBuildings(true)
    {
    }

    ~RenderMain() {
      AppStatusManager::RemoveEventHandler(this);
    }

    bool initialize() {
      try {
        // 地表グリッド線を書く
        for (float64_t x = MapClipRect.left; x < MapClipRect.right(); x += 100.0f) {
          shared_ptr<Line3D> lineV(new Line3D(Vector3Dd(x, MapClipRect.top, -1.0), Vector3Dd(x, MapClipRect.bottom(), -1.0), ColorRGBA(120,120,120,55)));
          m_grid->addChild(lineV);
        }
        for (float64_t y = MapClipRect.top; y < MapClipRect.bottom(); y += 100.0f) {
          shared_ptr<Line3D> lineH(new Line3D(Vector3Dd(MapClipRect.left, y, -1.0), Vector3Dd(MapClipRect.right(), y, -1.0), ColorRGBA(120,120,120,55)));
          m_grid->addChild(lineH);
        }
        m_grid->setZoffset(-10.0f);
        m_world->addChild(m_grid);
        m_world->addChild(m_north);
        m_world->addChild(m_south);
        m_world->addChild(m_west);
        m_world->addChild(m_east);
        m_world->addChild(m_top);
        AppStatusManager::AddEventHandler(this);
        m_south->disable();
        m_west->disable();
        m_east->disable();
        m_top->disable();
      }
      catch(...) {
        return false;
      }
      return true;
    }

    // AppStatusManagerから呼ばれるイベント
    virtual void onSetAppMode(AppMode mode) {
      ScopedLock lock(m_mutex);
      switch(mode) {
      case APP_MAP_VIEW_MODE:
        m_viewControl.setCameraMode(ViewControl::SEE_FREE);
        m_editView = false;
        m_enableBuildings = true;
        break;
      case APP_EDIT_BUILDING_MODE:
        m_editView = true;
        enableGroundMap(true);
        m_enableBuildings = true;
        m_viewControl.setCameraMode(ViewControl::EDIT_BUILDING);
        break;
      case APP_CALC_CAMERAPOS_MODE:
        m_viewControl.setCameraMode(ViewControl::CALC_CAMERAPOS);
        m_editView = false;
        enableGroundMap(false);
        //m_enableBuildings = false;
        break;
      case APP_EDIT_TEXTURE_MODE:
        m_editView = true;
        enableGroundMap(true);
        m_enableBuildings = true;
        break;
      }
      m_viewControl.setRedrawRequest();
    }

    // マップの方角が変更された
    virtual void onSetMapDirection(MapDirection dir) {
      m_north->disable();
      m_south->disable();
      m_west->disable();
      m_east->disable();
      m_top->disable();
      switch(dir) {
      case MAPDIR_NORTH: m_north->enable(); break;
      case MAPDIR_SOUTH: m_south->enable(); break;
      case MAPDIR_EAST: m_east->enable(); break;
      case MAPDIR_WEST: m_west->enable(); break;
      case MAPDIR_TOP: m_top->enable(); break;
      default:
        break;
      }
      m_viewControl.seeDirection(dir);
      m_viewControl.setRedrawRequest();
    }


	// 描画
    void render(shared_ptr<IGraphicsContext> gc, shared_ptr<IRenderer> gl, const RectS32& viewPort) {
      m_redrawRequest = false;
      AppMode appMode = AppStatusManager::GetAppMode();
      ScopedLock lock(m_mutex);
      MeasureTime timer;
      timer.start();
      if (appMode == APP_MAP_VIEW_MODE) {
        gc->setClearColor(ColorRGBA(230,230,220,255));
      }
      else {
        gc->setClearColor(ColorRGBA(0,0,0,255));
      }
      gc->beginFrame();

      RectS32 v1(viewPort);
      RectS32 v2(viewPort);
      RectS32 v3(viewPort);
      if (m_editView) { 
        // メイン画面
        v1.width -= viewPort.height/2 + 2;

        // 右上画面
        v2.left = v2.width - viewPort.height/2 + 1;
        v2.width = viewPort.height/2 - 1;
        v2.top = v2.height-viewPort.height/2 + 1;
        v2.height = viewPort.height/2 - 1;

        // 右下画面
        v3.left = v3.width - viewPort.height/2 + 1;
        v3.width = viewPort.height/2 - 1;
        v3.top = 0;
        v3.height = viewPort.height/2 - 1;
      }
      


      switch (appMode) {
      case APP_EDIT_BUILDING_MODE:
        drawMainView(gc.get(), gl.get(), v1);
        drawTopView(gc.get(), gl.get(), v2);
        drawTextureView(gc.get(), gl.get(), v3);
        break;
      case APP_EDIT_TEXTURE_MODE:
        drawEditTextureView(gc.get(), gl.get(), v1);
        drawPreView(gc.get(), gl.get(), v2);
        drawTextureView(gc.get(), gl.get(), v3);
        break;
      default:
        drawMainView(gc.get(), gl.get(), v1);
        break;
      }

      // 最後に枠を書く
      if (m_editView) {
        gc->setViewPort(viewPort);
        gl->selectProjectionMatrix();
        gl->loadIdentity();
        gl->ortho(viewPort.left, viewPort.right(), viewPort.top, viewPort.bottom(), -1.0 , 1.0);
        gl->enableDepthTest(false);
        gl->selectModelViewMatrix();
        gl->loadIdentity();
        gl->drawLine2D(Vector2D(v1.right(), v1.top), Vector2D(v1.right(), v1.bottom()), ColorRGBA(255,255,255,255));
        gl->drawLine2D(Vector2D(v1.right()+2, v1.top), Vector2D(v1.right()+2, v1.bottom()), ColorRGBA(70,70,70,255));
        gl->drawLine2D(Vector2D(v1.right()+1, v1.cy()+1), Vector2D(viewPort.right(), v1.cy()+1), ColorRGBA(255,255,255,255));
        gl->drawLine2D(Vector2D(v1.right()+1, v1.cy()-1), Vector2D(viewPort.right(), v1.cy()-1), ColorRGBA(70,70,70,255));
      }

      gc->endFrame();
      timer.stop();
      mstring msg;
      msg.format("1:%.2lffps", 1.0 / timer.getTotalTime());
      AppStatusManager::SetStatusText(msg);

      m_viewControl.setRedrawRequest(m_redrawRequest);
    }

    void drawMainView(IGraphicsContext* gc, IRenderer* gl, const RectS32& viewPort) {

      m_viewControl.setViewPort(viewPort);
      m_viewControl.beginFrame(gc, gl, viewPort);

      Matrix pm, mm, mmm;
      gl->getProjectionMatrix(pm);
      gl->getModelViewMatrix(mm);
      mmm = pm * mm;
      m_viewControl.setViewMatrix(mmm);

      if (m_groundMap) {
        Matrix groundMatrix;
        groundMatrix.translate(m_viewControl.objectOffset());
        m_ground->setTransform(groundMatrix);
        m_ground->render(gl);
      }
      gl->enableDepthTest(false);

      m_world->render(gl);

      if (m_show2D) {
        show2D(gl, mmm, viewPort);
        m_viewControl.beginFrame(gc, gl, viewPort);
        gl->clearDepthBuffer();
      }

      gl->enableDepthTest(true);
      if (m_enableBuildings) {
        m_bc.set2DMatrix(mmm);
        m_bc.get3DObject()->render(gl);
      }
      m_viewControl.endFrame(gc, gl);
    }

    void drawPreView(IGraphicsContext* gc, IRenderer* gl, const RectS32& v2) {

      m_viewControl.setRightTopViewPort(v2);
      m_viewControl.beginFrame(gc, gl, v2);

      Matrix pm, mm, mmm;
      gl->getProjectionMatrix(pm);
      gl->getModelViewMatrix(mm);
      mmm = pm * mm;

      if (m_groundMap) {
        Matrix groundMatrix;
        groundMatrix.translate(m_viewControl.objectOffset());
        m_ground->setTransform(groundMatrix);
        m_ground->render(gl);
      }

      //m_world->render(gl);
      m_bc.set2DMatrix(mmm);
      m_bc.get3DObject()->render(gl);

      m_viewControl.endFrame(gc, gl);
    }

	void drawTopView(IGraphicsContext* gc, IRenderer* gl, const RectS32& v2) {
      // トップビュー画面
      m_viewControl.setRightTopViewPort(v2);
      gc->setViewPort(v2);
      gl->selectProjectionMatrix();
      gl->loadIdentity();
      gl->ortho(-100.0 , 100.0 , -100.0 , 100.0 , 0.1 , 100000.0);
      gl->selectModelViewMatrix();
      gl->loadIdentity();
      gl->lookAt(Vector3D(0.0f,0.0f,1000.0f), Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,1.0f,0.0f));
      BuildingObject::ptr bldg(m_bc.getCurrentObject());
      if (bldg.get() != 0) {
        gl->translation(Vector3D(0.0f,0.0f,0.0f) - bldg->getOrigin());
      }
      else {
        gl->translation(Vector3D(0.0f,0.0f,0.0f)-m_viewControl.camera().target);
      }
      if (m_groundMap) {
        m_ground->render(gl);
      }
      m_bc.get3DObject()->render(gl);
    }

    // 　テクスチャビュー右下画面
    void drawTextureView(IGraphicsContext* gc, IRenderer* gl, const RectS32& v2) {
      m_viewControl.setRightBottomViewPort(v2);
      gc->setViewPort(v2);
      gl->selectProjectionMatrix();
      gl->loadIdentity();
      gl->ortho(-1.0 , 1.0 , -1.0 , 1.0 , -10000.0 , 100000.0);
      gl->selectModelViewMatrix();
      gl->loadIdentity();
    //  gl->lookAt(Vector3D(0.0f,0.0f,1000.0f), Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,1.0f,0.0f));

      BuildingObject::ptr bldg(AppStatusManager::GetCurrentBuildingObject());
      if (bldg.get() != 0) {
        gobj::Texture2D::ptr tex(bldg->getTexture2D());
        gobj::Rectangle3D rect3D;
        rect3D.setTexture2D(tex);
        rect3D.render(gl);
      }
    }

      // 　テクスチャビューメイン画面
    void drawEditTextureView(IGraphicsContext* gc, IRenderer* gl, const RectS32& view) {
      m_viewControl.setViewPort(view);
      gc->setViewPort(view);
      gl->selectProjectionMatrix();
      gl->loadIdentity();
      gl->ortho(view.left, view.right(), view.bottom(),view.top,  -10000.0 , 100000.0);
      gl->selectModelViewMatrix();
      gl->loadIdentity();
      gl->enableDepthTest(false);
      m_bc.editTexture(m_viewControl.getDraggingRect(), m_viewControl.getMoveRectMode(), view, m_viewControl.getZoomRatio());
      m_bc.getEditTexObj()->render(gl);
      gl->enableDepthTest(true);
    }

    // 2D描画
    void show2D(IRenderer* gl, Matrix& mmm, const RectS32& viewPort) {
      MapDirection mapDir = AppStatusManager::GetMapDirection();

      gl->selectProjectionMatrix();
      gl->loadIdentity();
      gl->ortho(-1.0 , 1.0 , -1.0 , 1.0 , 0.1 , 100000.0);
      gl->selectModelViewMatrix();
      gl->loadIdentity();
      gl->lookAt(Vector3D(0.0f,0.0f,1000.0f), Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,1.0f,0.0f));

      const AeroPhotoObject::ptr mapObj = AppStatusManager::getCurrentAeroPhotoObject();
      bool drawMap = false;
      if (mapObj.get() != 0) {
        // 航空写真の2D描画
        gl->disableTexture2D();
        gl->color(ColorRGBA(255,0,0,255));
        gl->lineWidth(2.0f);
        gl->begin(IRenderer::LINE_LOOP);
        for (int n = 0; n < 4; ++n) {
          Vector3D r;
          r = mapObj->getVertex(n);
          r = mmm.mult2d(r);
          r.z = 990.0f;
          gl->vertex(r);
          m_mapRect2D[n].x = r.x;
          m_mapRect2D[n].y = r.y;
        }
        m_aspect2D = ((m_mapRect2D[2].y - m_mapRect2D[1].y)*m_viewControl.getViewPort().height)
                    / ((m_mapRect2D[1].x - m_mapRect2D[0].x)*m_viewControl.getViewPort().width);
        m_role2D   = m_mapRect2D[1].y - m_mapRect2D[0].y;
        gl->end();
        shared_ptr<Rectangle2D> rect(new Rectangle2D) ;
        rect->setVertexes(m_mapRect2D);
        rect->setTexture2D(mapObj->getTexture());
        rect->doRendering(gl);
        drawMap = true;
        // カメラ位置計算モードの場合、カメラ位置の計算を行う
        if (AppStatusManager::GetAppMode() == APP_CALC_CAMERAPOS_MODE) {
          if (m_viewControl.getCameraPositionCalculator().calcCameraPos(viewPort)) {
            // おしまい
            //AppStatusManager::SetAppMode(APP_MAP_VIEW_MODE);
          }
        }
      }
      if (m_bc.isMakeTextureMode()) {
        // テクスチャ生成モード
        uint32_t cnt = 0; // とりあえず16個ずつ作る
        while (cnt < 16 && m_bc.isMakeTextureMode()) {
          BuildingObject::ptr bldg(m_bc.getMakeTextureBuilding());
          if (bldg.get() != 0) {
            // 建築物の2D描画
            bldg->setRenderingMatrix(mmm);
            app::IBuilding3D::ptr bldg3D = bldg->getObject();
            BoundaryBox2D<float32_t> bb;
            bb.set(m_mapRect2D[0]);
            bb.set(m_mapRect2D[1]);
            bb.set(m_mapRect2D[2]);
            bb.set(m_mapRect2D[3]);
            if (bb.getRect().isInside(bldg3D->getBoundary2D().left, bldg3D->getBoundary2D().top)) {
              MeasureTime time;
              time.start();

              bldg3D->setRectColor(ColorRGBA(255,255,0,255));
              bldg3D->makeRectangle2D(mmm);
              if (bldg3D->makeTexture(mapObj->getImage(), m_mapRect2D, mapDir)) {
                bldg->setTexture2D();
                bldg->clearMidified();
              }

              time.stop();
              mstring text;
              text.format("0:make texture time:%.3lfms", time.getTotalTime() * 1000.0);
              AppStatusManager::SetStatusText(text);
              ++cnt;
            }
          }
        }
        m_redrawRequest = true;
      }
      else {
        BuildingObject::ptr bldg(AppStatusManager::GetCurrentBuildingObject());
        if (bldg.get() != 0 && bldg->getStatus() == BuildingObject::SELECTED) {
          // 建築物の2D描画
          bldg->setRenderingMatrix(mmm);
          app::IBuilding3D::ptr bldg3D = bldg->getObject();
          bldg3D->setRectColor(ColorRGBA(255,255,0,255));
          bldg3D->makeRectangle2D(mmm);
          if (drawMap != 0 && bldg->isModified()) {
            if (bldg3D->makeTexture(mapObj->getImage(), m_mapRect2D, mapDir)) {
              bldg->setTexture2D();
              bldg->clearMidified();
            }
          }
          bldg3D->enableBorder(true);
          bldg3D->enableSolid(false);
          bldg3D->setLineWidth(3.0f);
          bldg3D->doRenderingRect(gl, m_viewControl.getSizePerPixel());
          bldg3D->setLineWidth(1.0f);
        }
      }
    }

    void enableEditView(bool enable = true) {
      m_editView = enable;
    }

    void enableGroundMap(bool enable = true) {
      m_groundMap = enable;
      m_viewControl.setRedrawRequest();
    }


    float32_t getAspect2D() const { return m_aspect2D; }

    float32_t getRole2D() const { return m_role2D; }

    
    ViewControl& getViewControl() { return m_viewControl; }

    const ViewControl& getViewControl() const { return m_viewControl; }

    void enableGrid(bool enable = true) { 
      m_grid->enable(enable); 
      m_viewControl.setRedrawRequest();
    }
    void enableImage2D(bool enable = true) {
      m_show2D = enable; 
      m_viewControl.setRedrawRequest();
    }
    void enableWireFrame(bool enable = true) {
      m_bc.setWireFrame(enable);
      m_viewControl.setRedrawRequest();
    }
    void setGround(gobj::Group::ptr ground) { m_ground = ground; }

    const RectVector2D& getMapRect2D() const { return m_mapRect2D; }

    void addGraphicsObject(gobj::Group::ptr& obj, MapDirection dir) {
      switch(dir) {
      case MAPDIR_NORTH:
        m_north->addChild(obj); break;
      case MAPDIR_SOUTH:
        m_south->addChild(obj); break;
      case MAPDIR_EAST:
        m_east->addChild(obj); break;
      case MAPDIR_WEST:
        m_west->addChild(obj); break;
      case MAPDIR_TOP:
        m_top->addChild(obj); break;
      }
    }

  private:
    gobj::Group::ptr getWorld() { return m_world; }


  private:
    BuildingCreator& m_bc;
    gobj::Group::ptr m_world;
    gobj::Group::ptr m_north;
    gobj::Group::ptr m_south;
    gobj::Group::ptr m_west;
    gobj::Group::ptr m_east;
    gobj::Group::ptr m_top;
    gobj::Group::ptr m_grid;
    gobj::Group::ptr m_ground;
    ViewControl m_viewControl;
    bool m_show2D;
    float32_t m_aspect2D;
    float32_t m_role2D;
    bool m_editView;
    bool m_groundMap;
    Mutex m_mutex;
    bool m_enableBuildings;
    RectVector2D m_mapRect2D;
    bool m_redrawRequest;
  };







} // namespace

#endif
