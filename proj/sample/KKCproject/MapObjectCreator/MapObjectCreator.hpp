// MapObjectCreator
//
// メインアプリケーションクラス

#ifndef MapObjectCreator_HPP
#define MapObjectCreator_HPP

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#include <iostream>
#endif
#include "Util/bstream.hpp"
#include "Util/ustring.hpp"
#include "Util/util.hpp"
#include "Graphics/Image/ImageBuffer.hpp"
#include "Graphics/Image/JpegImage.hpp"

#include "AppStatusManager.hpp"
#include "RenderMain.hpp"
#include "ShpFile.hpp"
#include "BuildingCreator.hpp"
#include "ViewControl.hpp"
#include "AeroPhotoObjectList.hpp"
#include "MouseEventHandler.hpp"
#include "EditTextureEventHandler.hpp"
#include "EditBuildingEventHandler.hpp"
#include "MapViewEventHandler.hpp"
#include "CalcCameraPosEventHandler.hpp"
#include "GroundMap.hpp"
#include "MocFileImporter.hpp"
#include "VrmlExporter.hpp"

#include "IUserInterfaceHandler.h"

namespace app {



  const Vector3D GrandMapOrigin(-7887.7239f, -32827.695f, 0.0f);
  const Vector3D MarionOrigin(-6215.06347f, -36454.01953f, 0.0f);
  const Vector3D GldScale(18.018326f, 21.985806f, 21.0f);

  class MapObjectCreator : private IAppStatusEventHandler, public IUserInterfaceHandler {
  public:

    MapObjectCreator()
      : m_renderMain(m_building)
      , m_mapViewEventHandler(new MapViewEventHandler(getViewControl(), m_building, m_aeroPhotoObjectList))
      , m_calcCameraPosEventHandler(new CalcCameraPosEventHandler(getViewControl(), m_building))
      , m_editBuildingEventHandler(new EditBuildingEventHandler(getViewControl(), m_building))
      , m_editTextureEventHandler(new EditTextureEventHandler(getViewControl(), m_building))
    {}

    ~MapObjectCreator() {
      AppStatusManager::RemoveEventHandler(this);
    }

    // MapObjectCreatorはシングルトン
    static MapObjectCreator& getInstance() {
      static MapObjectCreator myInstance;
      return myInstance;
    }

    virtual bool initialize() {
      if (!m_renderMain.initialize()) return false;
      AppStatusManager::AddEventHandler(this);
      AppStatusManager::SetAppMode(APP_MAP_VIEW_MODE);
      return true;
    }

    // AppStatusManagerから呼ばれるイベント
    virtual void onSetAppMode(AppMode mode) {
      switch(mode) {
      case APP_MAP_VIEW_MODE:
        setMouseEventHandler(m_mapViewEventHandler);
        m_uiEventHandler = IUiEventHandler::ptr();
        m_building.setWireFrame(false);
        m_building.unsetAllObject();
        break;
      case APP_EDIT_BUILDING_MODE:
        setMouseEventHandler(m_editBuildingEventHandler);
        m_uiEventHandler = m_editBuildingEventHandler;
        break;
      case APP_CALC_CAMERAPOS_MODE:
        setMouseEventHandler(m_calcCameraPosEventHandler);
        m_uiEventHandler = IUiEventHandler::ptr();
        m_building.unsetAllObject();
        break;
      case APP_EDIT_TEXTURE_MODE:
        setMouseEventHandler(m_editTextureEventHandler);
        m_uiEventHandler =  m_editTextureEventHandler;
        m_building.setWireFrame(false);
        break;
      }
    }

    // マップの方角が変更された
    virtual void onSetMapDirection(MapDirection dir) {
      switch (AppStatusManager::GetAppMode()) {
      case APP_EDIT_BUILDING_MODE:
        {
          BuildingObject::ptr bldg(m_building.getCurrentObject());
          if (bldg.get() == 0) return;
          uint32_t mapId = m_aeroPhotoObjectList.findByMapPosition(bldg->getOrigin(), dir, true);
          if (mapId == AeroPhotoObjectList::InvalidId) return;
          m_aeroPhotoObjectList.setCurrentAeroPhotoObject(mapId);
          AeroPhotoObject::ptr mapObj(m_aeroPhotoObjectList.getAeroPhotoObject(mapId));
          mapObj->setStatus(AeroPhotoObject::Visible);
          getViewControl().setCameraPos(mapObj->getCameraPos());
        }
        break;
 
      default:
        break;
      }


    }


    // MOCファイルの読み込み
    virtual void mocFileOpen(const mstring& filename) {

      MocFileImporter().mocFileOpen(filename, m_aeroPhotoObjectList, m_renderMain);

      // 地表データの読み込み
      m_groundMap.initialize(Importer::GetRootDirectory().body());
      m_renderMain.setGround(m_groundMap.get3DObject());
      m_renderMain.getViewControl().objectOffset() = GrandMapOrigin; 

      // Viewの設定
      getViewControl().setCameraMode(ViewControl::TOP_DOWN);

      // ビリディング基本データの読み込み
      m_building.loadConfiguration(Importer::GetRootDirectory());

    }

    // shpファイルを開く
    virtual void shpFileOpen(const mstring& filename) {
      if (m_listDialog.get() == 0) throw std::runtime_error("IListDialog is null");

      ifstream inf(filename.c_str(), ios_base::binary);
      ShpFile shp(inf);

      // .dbfファイルの読み込み
      mstring dbfn(filename);
      dbfn.replace_string(".shp", ".dbf");
      ifstream dbf(dbfn.c_str());
      DBaseFile dbase(dbf);

      // .dbfファイルの項目をリストビューに登録する
      if (m_listDialog->getColumnCount() == 0) {
        for (size_t field = 0; field < dbase.getFieldCount(); ++field) {
          const DBaseFile::Field& f = dbase.getField(field);
          m_listDialog->addColumnTitle(f.length * 8, f.name);
        }
        m_listDialog->update();
      }

      // データディレクトリの生成
      mstring dataDir(util::getDirectoryName(dbfn));
      // shpディレクトリだったらひとつ上にする。
      if (dataDir.rightMatch("shp\\") || dataDir.rightMatch("shp/") ) {
        dataDir = dataDir.substr(0, dataDir.length() - 1);
        dataDir = util::getDirectoryName(dataDir);
      }

      // マップデータの読み込み
      MocFileImporter().mocFileOpen(filename, m_aeroPhotoObjectList, m_renderMain, shp, dbase);

      // 地表データの読み込み
      m_groundMap.initialize(dataDir);
      m_renderMain.setGround(m_groundMap.get3DObject());
      m_renderMain.getViewControl().objectOffset() = Vector3D(-7887.7239f, -32827.695f, 0.0f);

      // Viewの設定
      getViewControl().setCameraMode(ViewControl::TOP_DOWN);

      // ビリディング基本データの読み込み
      m_building.loadConfiguration(Importer::GetRootDirectory());
    }

    // ビルディングファイルの読み込み
    virtual void bldFileOpen(const mstring& filename) {
      // データディレクトリの取得
      mstring dataDir(util::getDirectoryName(mstring(filename)));
      TextureManager::SetDirectory(dataDir + "image/");

      sys::PushMouseCursor(sys::WAIT_CURSOR);

      // マップデータがロードされていない場合は読み込む
      if (m_aeroPhotoObjectList.isEmpty()) {
        mocFileOpen(dataDir + "MapData.moc");
      }

      // ビルディングデータの読み込み
      ifstream ifs(filename.c_str(), ios_base::binary);
      m_building.setFileName(filename.body());
      if (ifs.is_open()) {
        AppStatusManager::SetStatusText("0:Loding Building files...");
        m_building.read(ifs);
        AppStatusManager::SetStatusText("0:");
      }
      sys::PopMouseCursor();
    }

    // GLDテキストファイルのインポート
    virtual void gldFileOpen(const mstring& filename) {
      ifstream ifs(filename.c_str(), ios_base::binary);
      uint32_t cnt = 0;
      uint32_t polygons = 0;

      while (!ifs.eof()) {
        string linestr;
        getline(ifs, linestr, '\r');
        const char* line = linestr.c_str();
        if (strncmp(line, "BLD@", 4) ==0) {
          BuildingObject::ptr build(new BuildingObject(BuildingObject::FREE_BUILDING, Vector3Dd(MarionOrigin.x, MarionOrigin.y, 0.0))); // , GrandMapOrigin.x, GrandMapOrigin.y));
          FreeBuilding3D::ptr gobj = dynamic_pointer_cast<FreeBuilding3D>(build->getObject());

          mstring buf(line+5);
          char type = line[4];
          vector<string> svtx;
          buf.split(svtx, ',');
          vector<Vector3D> vertex;
          for (vector<string>::iterator i = svtx.begin(); i != svtx.end(); ++i) {
            string& s = *i;
            if (s[0] == '*') { // end object
              switch (type) {
              case 'Q':
                gobj->addQuads(vertex);
                break;
              case 'T':
                gobj->addTriangles(vertex);
                break;
              case 'S':
                gobj->addTriangleStrip(vertex);
                break;
              default:
                TRACE("Unknown Type %c\n", type);
              }
              vertex.clear();
              type = s[1];
              if (s.length() < 3) break;
              s = s.substr(2);
            }
            float x, y, z;
            sscanf_s(s.c_str(), "%f %f %f", &x, &y, &z);
            if (z < 0.0 || z > 50.0) z = 0; // data bug?
            vertex.push_back(Vector3D(x, -y, z) * GldScale);
            ++polygons;
          }
          //gobj->setScale(GldScale);
          gobj->vertexNormalization(); // 頂点座標の適正化
          m_building.addBuildingObject(build);
        }
        if ((++cnt % 64) == 0) {
          AppStatusManager::SetStatusText(mstring().format("0:imputing building: %d models %d polygons", cnt, polygons));
        }
      }
      AppStatusManager::SetStatusText(mstring().format("0:%d building models imported. (%d polygons)", cnt, polygons));
    }

    virtual void exportBuldingToVrml(mstring& fn) {
      BuildingObject::ptr building(AppStatusManager::GetCurrentBuildingObject());
      if (building.get() != 0) {
        VrmlExporter().exportBuilding(fn, building);
      }
    }

    //
    // イベントハンドラ
    //

    // 建物の保存
    void onButtonSaveBuilding(const mstring& fn) {
      m_building.setFileName(fn);
      if (m_building.getFileName().empty()) {
        m_building.setFileName("building.bld");
      }
      ofstream ofs(m_building.getFileName().c_str(), ios_base::binary);
      if (ofs.is_open()) {
        m_building.write(ofs);
      }
      else {
        throw std::runtime_error("write error");
      }
      // テクスチャも保存
      TextureManager::SaveAllTexture();
    }


    // "go top" ボタンが押された
    void onButtonGoTop() {
      AppStatusManager::SetAppMode(APP_MAP_VIEW_MODE);
      m_building.setWireFrame(false);
 
      getViewControl().setRedrawRequest();
      getViewControl().setCameraMode(ViewControl::TOP_DOWN);
    }

    // カメラの位置の視点をセット
    bool adjustAeroPhotoCameraPosition() {
      AeroPhotoObject::ptr mapObj(AppStatusManager::getCurrentAeroPhotoObject());
      if (mapObj.get() == 0) return false;
      if (mapObj->getCameraPos().eye.x != 0.0 && mapObj->getCameraPos().eye.y != 0.0) {
        getViewControl().setCameraPos(mapObj->getCameraPos());
        return true;
      }
      else {
        // カメラ位置の計算
        onButtonRecalcCamera();
      }
      return false;
    }

    // カメラ位置再計算ボタンが押された
    void onButtonRecalcCamera() {
      AeroPhotoObject::ptr mapObj(AppStatusManager::getCurrentAeroPhotoObject());
      if (mapObj.get() == 0) throw std::runtime_error("onButtonEditobj");
      // カメラ位置計算クラスに航空写真の四隅の座標をセット
      importCameraPos();
      getViewControl().getCameraPositionCalculator().startCalculate(mapObj->getCorners());
      AppStatusManager::SetAppMode(APP_CALC_CAMERAPOS_MODE);
    }

    // カメラ位置計算ボタンが押された
    void onButtonCalcCamera() {
      AeroPhotoObject::ptr mapObj(AppStatusManager::getCurrentAeroPhotoObject());
      if (mapObj.get() == 0) throw std::runtime_error("onButtonEditobj");
      if (mapObj->getCameraPos().eye.x != 0.0 && mapObj->getCameraPos().eye.y != 0.0) {
        // 保存されていたらそれをつかう
        getViewControl().setCameraPos(mapObj->getCameraPos());
        getViewControl().setRedrawRequest();
      }
      else {
        //onButtonRecalcCamera();
      }
    }

    // カメラ位置保存ボタンが押された
    void onButtonSaveCameraPos() {
      AeroPhotoObject::ptr mapObj(AppStatusManager::getCurrentAeroPhotoObject());
      if (mapObj.get() == 0) throw std::runtime_error("onButtonEditobj");
      mapObj->setCameraPos(getViewControl().getCamera());
      ofstream ofs((m_aeroPhotoObjectList.getDataDir() + "MapData.moc").c_str(), ios_base::binary);
      m_aeroPhotoObjectList.write(ofs);
    }

    // カメラ位置取り込みボタンが押された
    virtual void importCameraPos() {
      AeroPhotoObject::ptr mapObj(AppStatusManager::getCurrentAeroPhotoObject());
      if (mapObj.get() == 0) throw std::runtime_error("importCameraPos");
      CameraPosition cameraPos = getViewControl().getCamera();
      cameraPos.eye =  mapObj->getOriginalCamera();
      cameraPos.eye.z -= mapObj->getOriginalCameraElevation();
      cameraPos.target = CameraPositionCalculator::getCenterPoint(mapObj->getCorners());
      if (mapObj->getDirection() == MAPDIR_TOP) {
        cameraPos.top = Vector3Dd(0.0, 1.0, 0.0);
      }
      getViewControl().setCameraPos(cameraPos);
      getViewControl().setRedrawRequest();
    }

    // 編集ボタンが押された
    virtual void onButtonEditobj() {
      adjustAeroPhotoCameraPosition();
      AppStatusManager::SetAppMode(APP_EDIT_BUILDING_MODE);
      m_building.setWireFrame();
      getViewControl().setRedrawRequest();
    }

    // ビル追加ボタンが押された
    virtual void onButtonAddBuilding(uint32_t /*type*/) {
      AeroPhotoObject::ptr mapObj(AppStatusManager::getCurrentAeroPhotoObject());
      if (mapObj.get() == 0) throw std::runtime_error("onButtonEditobj");
      Vector3Dd pos = CameraPositionCalculator::getCenterPoint(mapObj->getCorners());
      BuildingObject::ptr building(new BuildingObject(BuildingObject::SIMPLE_BUILDING, Vector3Dd(pos.x, pos.y, 0.0)));
      building->setTexture( "building.jpg");
      m_building.addBuildingObject(building);
      getViewControl().setRedrawRequest();
    }

    // ダブルクリックされた
    virtual void onButtunDblClick() {
      if (AppStatusManager::GetAppMode() == APP_EDIT_BUILDING_MODE) {
        BuildingObject::ptr bldg = AppStatusManager::GetCurrentBuildingObject();
        if (bldg.get() != 0) {
          bldg->setModified();
          getViewControl().setRedrawRequest();
        }
      }
    }

    // 建物の分割ボタンが押された
    // 分割モードになったときはtrueを返す
    virtual bool onButtonCutBuilding() {
      if (AppStatusManager::GetAppMode() == APP_EDIT_BUILDING_MODE) {
        BuildingObject::ptr bldg = AppStatusManager::GetCurrentBuildingObject();
        if (bldg.get() != 0) {
          if (m_building.isCutBuildingMode()) {
            if (sys::ShowYesNoDialog("建物の分割を行いますか？")) {
              m_building.doCutBuilding();
              bldg->setModified();
            }
            m_building.setCutBuildingMode(false);
          }
          else {
            m_building.setCutBuildingMode();
            getViewControl().setRedrawRequest();
            return true;
          }
        }
      }
      getViewControl().setRedrawRequest();
      return false;
    }

    // 建物へのキューブ追加
    virtual bool onButtonAddCube() {
      if (AppStatusManager::GetAppMode() != APP_EDIT_BUILDING_MODE) return false;
      if (m_building.addCubeObject()) {
        getViewControl().setRedrawRequest();
        return true;
      }
      return false;

    }


    virtual void onButtonViewMode() {
      AppStatusManager::SetAppMode(APP_MAP_VIEW_MODE);
    }

    virtual void onButtonEditTextureMode() {
      AppStatusManager::SetAppMode(APP_EDIT_TEXTURE_MODE);
    }


    virtual void onSetMousePosition(MouseStatus& ms) {
      m_mouseEvent.eventDispatch(ms);
    }


    virtual void onZoomChange(int32_t zoomRatio) {
      m_renderMain.getViewControl().setZoomRatio(zoomRatio);
      getViewControl().setRedrawRequest();
    }

    virtual void onButtonCopy() {
      if (m_uiEventHandler.get() != 0) m_uiEventHandler->onButtonCopy();
    }
    virtual void onButtonPaste() {
      if (m_uiEventHandler.get() != 0) m_uiEventHandler->onButtonPaste();
    }

    virtual void onMakeTexture() {
      if (AppStatusManager::GetAppMode() == APP_EDIT_BUILDING_MODE) {
        m_building.setMakeTextureMode();
        getViewControl().setRedrawRequest();
      }
    }

    // 一定期間毎に呼ばれる
    virtual void onTimer() {
      if (AppStatusManager::GetAppMode() == APP_CALC_CAMERAPOS_MODE && m_renderMain.getViewControl().getCalcPhase() == 0) {
        // カメラ位置計算モードの終了
        AppStatusManager::SetAppMode(APP_MAP_VIEW_MODE);
      }
    }

    // RenderMain系
    virtual void render(shared_ptr<IGraphicsContext> gc, shared_ptr<IRenderer> gl, const RectS32& viewPort) {
      m_renderMain.render(gc, gl, viewPort);
    }
    virtual float32_t getAspect2D() {
      return m_renderMain.getAspect2D();
    }
    virtual float32_t getRole2D() {
      return m_renderMain.getRole2D();
    }
    virtual void enableImage2D(bool enable) {
      m_renderMain.enableImage2D(enable);
    }
    virtual void enableGrid(bool enable) {
      m_renderMain.enableGrid(enable);
    }
    virtual void enableWireFrame(bool enable) {
      m_renderMain.enableWireFrame(enable);
    }
    virtual void enableGroundMap(bool enable) {
      m_renderMain.enableGroundMap(enable);
    }

    // ViewControl系
    virtual void  setCameraPos(const CameraPosition& cam) {
      m_renderMain.getViewControl().setCameraPos(cam);
    }
    virtual const CameraPosition& getCamera() const {
      return m_renderMain.getViewControl().getCamera();
    }
    virtual bool  getRedrawRequest() const {
      return m_renderMain.getViewControl().getRedrawRequest();
    }
    virtual void  setRole(float32_t role) {
      m_renderMain.getViewControl().setRole(role);
    }
    virtual void  setRedrawRequest() {
      m_renderMain.getViewControl().setRedrawRequest();
    }

    // AeroPhotoObjectList系
    virtual void setCurrentAeroPhotoObject(uint32_t mapId) {
      m_aeroPhotoObjectList.setCurrentAeroPhotoObject(mapId);
    }
    virtual void setAeroPhotoObjectVisible(uint32_t mapId) {
      m_aeroPhotoObjectList.setStatus(mapId, AeroPhotoObject::Visible);
    }
    virtual void setAeroPhotoObjectWireFrame(uint32_t mapId) {
      m_aeroPhotoObjectList.setStatus(mapId, AeroPhotoObject::WireFrame);
    }


    // setter/getter
    void setMouseEventHandler(shared_ptr<IMouseEventHandler> eh) { 
      m_mouseEvent.eventHandler = eh;
    }

    AeroPhotoObjectList& getAeroPhotoObjectList() { return m_aeroPhotoObjectList; }
    RenderMain& getRenderMain() { return m_renderMain; }

    virtual void setListDialog(IListDialog::ptr listDialog) { m_listDialog = listDialog; }
    ViewControl& getViewControl() { return m_renderMain.getViewControl(); }
    BuildingObjectList::ptr getBuildingObjectList() { return m_building.getObjectList(); }

  private:
    BuildingCreator m_building;
    RenderMain m_renderMain;
    IListDialog::ptr m_listDialog;
    AeroPhotoObjectList m_aeroPhotoObjectList;
    MapViewEventHandler::ptr m_mapViewEventHandler;
    CalcCameraPosEventHandler::ptr m_calcCameraPosEventHandler;
    EditBuildingEventHandler::ptr m_editBuildingEventHandler;
    EditTextureEventHandler::ptr m_editTextureEventHandler;
    GroundMap m_groundMap;
    MouseEvent m_mouseEvent;
    IUiEventHandler::ptr m_uiEventHandler;
  };



} // namespace
#endif
