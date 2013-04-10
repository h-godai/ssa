// BuildingCreator.hpp
//
//


#ifndef APP_BUILDINGCREATOR_HPP
#define APP_BUILDINGCREATOR_HPP

#include <fstream>
#include "BuildingObjectList.hpp"

namespace app {
  using std::ifstream;

  const char BuildingConfigurationFile[] = "building.cfg";
  const char BuildingCounterName[]       = "building_count";

  class BuildingCreator {
  public:
    BuildingCreator()
      : m_buildings(new BuildingObjectList)
      , m_hitVertex(false)
      , m_editTexObj(new gobj::Group)
      , m_texRect3D(new gobj::Rectangle3D)
      , m_selRect3D(new gobj::Rectangle3D)
      , m_singleObjectMode(false)
      , m_cutBuindingMode(false)

    {
      m_editTexObj->addChild(m_texRect3D);
      m_editTexObj->addChild(m_selRect3D);
    }
    ~BuildingCreator() {
      if (!m_configFileName.empty()) {
        m_config.set(BuildingCounterName, static_cast<int>(BuildingObject::GetLastObjectId()));
        std::ofstream ofs(m_configFileName.c_str(), ios_base::binary);
        if (ofs.is_open()) {
          m_config.save(ofs);
        }
      }
    }

    // データの初期化
    void clear() {
      m_buildings->clear();
    }

    void loadConfiguration(const mstring& dir) {
      m_configFileName = dir + BuildingConfigurationFile;
      std::ifstream ifs(m_configFileName.c_str(), ios_base::binary);
      if (ifs.is_open()) {
        m_config.load(ifs);
        BuildingObject::SetLastObjectId(static_cast<uint32_t>(m_config.getInt(BuildingCounterName)));
      }
    }

    // 建造物を追加する
    void addBuildingObject(BuildingObject::ptr& bldg) {
      m_buildings->addBuildingObject(bldg);
    }

    // 2D座標系からカレントオブジェクトの頂点にヒットしているか調べる
    bool hitCheckVertex2D(const Vector2D& vtx, const Vector2D& range) {
      BuildingObject::ptr bl(m_buildings->getCurrentObject());
      if (bl.get() == 0) { m_hitVertex = false; return false; } // カレントが無い
      m_hitVertex = bl->hitCheckVertex2D(vtx, range);
      return m_hitVertex;
    }

    bool isHitVertex() const {
      return m_hitVertex;
    }

 
    // 2D座標系からカレントオブジェクトをセットする
    bool setCurrentObjectFor2D(const Vector2D& xy) {
      if (m_singleObjectMode && !isNull(m_buildings->getCurrentObject())) {
        // 単独モードで選択されている場合
        if (m_buildings->getCurrentObject()->getObject()->isPointInRect2D(xy)) {
          setSingleObjectMode(false);
          return false;
        }
        else {
          return false;
        }
      }
      else {
        if (m_buildings->setCurrentObjectFor2D(xy)) {
          // 設定成功
          setSingleObjectMode(true);
          return true;
        }
        else {
          setSingleObjectMode(false);
          return false;
        }
      }
    }

    void set2DMatrix(const Matrix& mmm) {
      m_buildings->set2DMatrix(mmm);
    }

    Group::ptr get3DObject() {
      setSingleObjectMode(m_singleObjectMode);
      return m_gobj;
    }

    void setWireFrame(bool enable = true) {
      m_buildings->setWireFrame(enable);
    }

    void read(istream& is) {
      bstream bs;
      bs.deserialize(is);
      read(bs);
    }

    void read(const bstream& bs) {
      clear();
      m_buildings->read(bs);
    }

    void write(ostream& os) {
      bstream bs;
      write(bs);
      bs.serialize(os);
    }

    void write(bstream& bs) {
      m_buildings->write(bs);
    }

    void setFileName(const string& fn) {
      m_filename = fn;
    }
    const string& getFileName() const {
      return m_filename;
    }


    BuildingObjectList::ptr getObjectList() { return m_buildings; }



    // ビルの位置を移動
    void moveCurrentBuilding(const Vector3D& v) {
      BuildingObject::ptr bl(m_buildings->getCurrentObject());
      if (bl.get() == 0) return;
      bl->addOrigin(v);
    }

    // 分割位置の移動
    void moveCutSurface(const Vector3D& v) {
      BuildingObject::ptr bl(m_buildings->getCurrentObject());
      if (bl.get() == 0) return;
      bl->moveCutSurface(v);
    }

    // ビルの頂点を移動して拡大・縮小を行う
    void moveCurrentVertex(const Vector3D& v) {
      BuildingObject::ptr bl(m_buildings->getCurrentObject());
      if (bl.get() == 0 || !m_hitVertex) return; // 頂点が選択されていない
      bl->moveVertex(v);
    }

    BuildingObject::ptr getCurrentObject() {
      return m_buildings->getCurrentObject();
    }

    // メインテクスチャの作成
    void editTexture(const RectS32& draggingRect, int moveRect, const RectS32& view, int32_t zoomRatio) {

      BuildingObject::ptr bldg(getCurrentObject());
      if (bldg.get() == 0) return;
      gobj::Texture2D::ptr tex(bldg->getTexture2D());
      if (tex.get() == 0) return;
      ImageBuffer::ptr img(tex->getImageBuffer());
      if (img.get() == 0) return;
      m_texRect3D->setTexture2D(tex);
      RectF32 texRect(bldg->getCurrentTextureCoord());
      m_texRect3D->setTexCoord(texRect);
      Vector2D texSize(img->getWidth() * texRect.width, img->getHeight() * texRect.height);
      float32_t zoom = pow(2.0f, zoomRatio - 1.0f);
      texSize *= zoom;
      float32_t x = (view.width - texSize.x) / 2.0f;
      float32_t y = (view.height - texSize.y) / 2.0f;
      m_texRect3D->setVertex(0, Vector3D(x+ texSize.x, y, 0.0f));
      m_texRect3D->setVertex(1, Vector3D(x,            y, 0.0f));
      m_texRect3D->setVertex(2, Vector3D(x,            y + texSize.y, 0.0f));
      m_texRect3D->setVertex(3, Vector3D(x+ texSize.x, y + texSize.y, 0.0f));

      // 選択範囲を描く
      RectF32 rect(draggingRect);
      m_texRect3D->setColor(ColorRGBA(255,255,255,255));
      if (rect.isEmpty()) {
        m_texRect3D->enable();
        m_selRect3D->disable();
      }
      else {
        gobj::Texture2D::ptr dragTex(new gobj::Texture2D());
        rect.correct();
        // 矩形をテクスチャの大きさに制限する
        if (rect.left < x) rect.left = x;
        if (rect.top < y) rect.top = y;
        if (moveRect != 0 && m_draggingImg.get() != 0) {
          rect.width = m_draggingTexRect.width * zoom;
          rect.height = m_draggingTexRect.height * zoom;
          if (rect.right() > x + texSize.x) rect.left = x + texSize.x - rect.width;
          if (rect.bottom() > y + texSize.y) rect.top = y + texSize.y - rect.height;
        }
        else {
          if (rect.right() > x + texSize.x) rect.width =  texSize.x - (rect.left - x);
          if (rect.bottom() > y + texSize.y) rect.height = texSize.y - (rect.top - y);
        }
        m_selRect3D->setVertexes(rect);
        rect.left -= x;
        rect.top -= y;
        rect.left /= zoom;
        rect.top /= zoom;
        rect.width /= zoom;
        rect.height /= zoom;
        m_draggingTexRect = RectS32(rect);

        if (moveRect != 0 && m_draggingImg.get() != 0) {
          dragTex->setImageBuffer(m_draggingImg);
          m_selRect3D->setTexture2D(dragTex);
          if (moveRect == 1) {
            // ドラッグ中
            m_texRect3D->setColor(ColorRGBA(255,255,255,255));
            m_selRect3D->setColor(ColorRGBA(255,255,255,190));
          }
          else {
            // ドラッグ後
            m_selRect3D->setColor(ColorRGBA(255,255,255,255));
            m_texRect3D->setColor(ColorRGBA(255,255,255,200));
          }
          m_texRect3D->enable();
          m_selRect3D->enable(); 
        }
        else {

          m_draggingTexRect.left += static_cast<int32_t>(texRect.left * img->getWidth());
          m_draggingTexRect.top += static_cast<int32_t>(texRect.top * img->getHeight());

          m_draggingImg = ImageBufferARGB::ptr(new ImageBufferARGB(m_draggingTexRect.width, m_draggingTexRect.height));
          try {
            m_draggingImg->bitblt(0,0, img.get(), m_draggingTexRect);
            dragTex->setImageBuffer(m_draggingImg);
            m_selRect3D->setTexture2D(dragTex);
            m_selRect3D->setColor(ColorRGBA(255,255,255,255));
            m_texRect3D->setColor(ColorRGBA(255,255,255,128));
            m_texRect3D->enable();
            m_selRect3D->enable(); 
          }
          catch (const std::exception&) {
            m_texRect3D->enable();
            m_selRect3D->disable();
          }
        }
      }
    }

    void pasteTextureImage() {
      BuildingObject::ptr bldg(getCurrentObject());
      if (bldg.get() == 0) return;
      gobj::Texture2D::ptr tex(bldg->getTexture2D());
      if (tex.get() == 0) return;
      ImageBuffer::ptr img(tex->getImageBuffer());
      if (img.get() == 0) return;

      RectF32 texRect(bldg->getCurrentTextureCoord());
      int32_t x = m_draggingTexRect.left + static_cast<int32_t>(img->getWidth() * texRect.left + 0.5f);
      int32_t y = m_draggingTexRect.top  + static_cast<int32_t>(img->getHeight() * texRect.top + 0.5f);
      img->bitblt(x, y, m_draggingImg.get());
      tex->setImageBuffer(img);
      bldg->setTexture2D(tex);
    }


    gobj::Group::ptr getEditTexObj() { return m_editTexObj; }

    void unsetAllObject() {
      m_buildings->unsetAllObject();
      setSingleObjectMode(false);
    }

    void setMakeTextureMode() {
      if (!m_makeTextureMode) {
        m_makeTextureMode = true;
        m_currentBuilding = 0;
      }
    }
    BuildingObject::ptr getMakeTextureBuilding() {
      if (m_currentBuilding >= m_buildings->getObjectCount()) {
        m_makeTextureMode = false;
        return BuildingObject::ptr();
      }
      else {
        return m_buildings->getObject(m_currentBuilding++);
      }
    }
    bool isMakeTextureMode() const { return m_makeTextureMode; }

    void setSingleObjectMode(bool enable = true) {
      m_gobj = gobj::Group::ptr(new gobj::Group());
      if (!enable) {
        // すべての建物を表示する
        m_gobj->addChild(m_buildings->getGObjRoot());
      }
      else {
        // カレントの建物のみを表示する
        BuildingObject::ptr bldg(m_buildings->getCurrentObject());
        if (bldg) {
          m_gobj->addChild(bldg->getObject());
          m_buildings->setChildObject(m_gobj, bldg);
        }
        else {
          // カレントが無い場合はすべての建物を表示する
          m_gobj->addChild(m_buildings->getGObjRoot());
          enable = false;
        }
      }
      m_singleObjectMode = enable;
    }

    bool isSingleObjectMode() const { return m_singleObjectMode; }

    void setCutBuildingMode(bool enable = true) { 
      m_buildings->setCutBuildingMode(enable);
      m_cutBuindingMode = enable;
    }
    bool isCutBuildingMode() const {
      return m_cutBuindingMode;
    }
    void doCutBuilding() {
      BuildingObject::ptr bl(m_buildings->getCurrentObject());
      if (bl.get() == 0) return;
      BuildingObject::ptr newBldg = bl->doCutBuilding();
      if (!isNull(newBldg)) {
        addBuildingObject(newBldg);
      }
    }

    // 建物にキューブポリゴンを追加
    bool addCubeObject() {
      BuildingObject::ptr bl(m_buildings->getCurrentObject());
      if (isNull(bl)) return false;
      BuildingObject::ptr newBldg = bl->addCubeObject();
      if (isNull(newBldg)) return false;
      addBuildingObject(newBldg);
      return true;
    }

  private:
    BuildingObjectList::ptr m_buildings;
    string m_filename;
    bool m_hitVertex;
    ImageBufferARGB::ptr m_draggingImg;
    RectS32 m_draggingTexRect;
    gobj::Group::ptr m_editTexObj;
    gobj::Rectangle3D::ptr m_texRect3D;
    gobj::Rectangle3D::ptr m_selRect3D;
    bool m_makeTextureMode;
    uint32_t m_currentBuilding;
    mstring m_configFileName;
    mstring_map m_config;
    bool m_singleObjectMode;
    gobj::Group::ptr m_gobj;
    bool m_cutBuindingMode;
};

} // namespace

#endif
