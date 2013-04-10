// IUserInterfaceHandler.h

#pragma once

#include "Common/Common.h"
#include "Util/ustring.hpp"
#include "Math/CameraPosition.h"
#include "MouseControl.hpp"
#include "Graphics/IGraphicsContext.h"
#include "Graphics/IRenderer.h"
#include "Math/Rect.h"
#include "AeroPhotoObject.hpp"

namespace app {

  using ts::mstring;
  using ts::CameraPosition;
  using ts::IGraphicsContext;
  using ts::IRenderer;
  using ts::RectS32;
  using boost::shared_ptr;

  class IListDialog {
  public:
    typedef shared_ptr<IListDialog> ptr;
    virtual size_t getColumnCount() = 0;
    virtual void addColumnTitle(uint32_t length, const string& name) = 0;
    virtual void update() = 0;
    virtual void setListData(uint32_t mapId, AeroPhotoObject::ptr mapObj) = 0;
  };

  class IUserInterfaceHandler {
  public:

    virtual ~IUserInterfaceHandler() {}

    virtual bool initialize() = 0;

    virtual void onButtonAddBuilding(uint32_t) = 0;
    virtual void onButtonSaveBuilding(const mstring&) = 0;
    virtual void onSetMousePosition(MouseStatus& ) = 0;
    virtual void onTimer() = 0;
    virtual void onButtunDblClick() = 0;
    virtual void onButtonGoTop() = 0;
    virtual void onButtonCalcCamera() = 0;
    virtual void onButtonSaveCameraPos() = 0;
    virtual void onButtonEditobj() = 0;
    virtual void onButtonViewMode() = 0;
    virtual void onButtonEditTextureMode() = 0;
    virtual void onZoomChange(int32_t zoomRatio) = 0;
    virtual void onButtonCopy() = 0;
    virtual void onButtonPaste() = 0;
    virtual void onButtonRecalcCamera() = 0;
    virtual void onMakeTexture() = 0;
    virtual bool onButtonCutBuilding() = 0;
    virtual bool onButtonAddCube() = 0;

    virtual void bldFileOpen(const mstring& mfn) = 0;
    virtual void mocFileOpen(const mstring& mfn) = 0;
    virtual void shpFileOpen(const mstring& mfn) = 0;
    virtual void gldFileOpen(const mstring& mfn) = 0;
    virtual void importCameraPos() = 0;
    virtual void exportBuldingToVrml(mstring& mfn) = 0;

    // RenderMain系
    virtual void render(shared_ptr<IGraphicsContext> gc, shared_ptr<IRenderer> gl, const RectS32& viewPort) = 0;
    virtual float32_t getAspect2D() = 0;
    virtual float32_t getRole2D()  = 0;
    virtual void enableImage2D(bool) = 0;
    virtual void enableGrid(bool) = 0;
    virtual void enableWireFrame(bool) = 0;
    virtual void enableGroundMap(bool) = 0;

    // ViewControl系
    virtual void  setCameraPos(const CameraPosition&) = 0;
    virtual const CameraPosition& getCamera() const = 0;
    virtual bool  getRedrawRequest() const = 0;
    virtual void  setRole(float32_t role) = 0;
    virtual void  setRedrawRequest() = 0;

    // AeroPhotoObjectList系
    virtual void setCurrentAeroPhotoObject(uint32_t mapId) = 0;
    virtual void setAeroPhotoObjectVisible(uint32_t mapId) = 0;
    virtual void setAeroPhotoObjectWireFrame(uint32_t mapId) = 0;
    virtual void setListDialog(IListDialog::ptr) = 0;

  };

  // シングルトンアクセス用
  extern IUserInterfaceHandler& MainApp();

}

