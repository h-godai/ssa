// MocFileImporter.hpp

#ifndef APP_MOCFILEIMPORTER_HPP
#define APP_MOCFILEIMPORTER_HPP

#ifndef STDAFX_H
#include <string>
#include <fstream>
#endif

#include "Util/Logger.h"
#include "Math/CameraPositionCalculator.h"

#include "Importer.hpp"
#include "AeroPhotoObjectList.hpp"
#include "RenderMain.hpp"
#include "ShpFile.hpp"

namespace app {

  using std::string;
  using namespace ts;

  class MocFileImporter : public Importer {
  public:

    // MOCファイルを開く
    void mocFileOpen(const mstring& filename, AeroPhotoObjectList& aeroPhotoObjectList, RenderMain& renderMain) {
      // データディレクトリの生成
      m_rootDirectory = util::getDirectoryName(filename);
      aeroPhotoObjectList.setDataDir(m_rootDirectory);
      TextureManager::SetDirectory(m_rootDirectory + "image/");

#if 0
      // リストビューの項目を登録する
      if (m_listDialog->getColumnCount() == 0) {
        m_listDialog->addColumnTitle(128, "ファイル名");
        m_listDialog->update();
      }
#endif

      // マップデータの読み込み
      try {
        ifstream ifs(filename.c_str(), ios_base::binary);
        aeroPhotoObjectList.read(ifs);
        for (uint32_t id = 0; id < aeroPhotoObjectList.getAeroPhotoObjectCount(); ++id) {
            AeroPhotoObject::ptr mapObj(aeroPhotoObjectList.getAeroPhotoObject(id));
            mapObj->initialize();
            mapObj->setStatus(AeroPhotoObject::WireFrame);
            renderMain.getViewControl().addBoundary(mapObj->getVertex());
            renderMain.addGraphicsObject(mapObj->getGraphicsObject(), mapObj->getDirection());
      //      m_listDialog->setListData(id, mapObj);        
        }
      }
      catch (const std::exception&) {
        aeroPhotoObjectList.clear();
        AfxMessageBox("MOCデータの読み込みに失敗しました");
      }
    }


    // MOCファイルをshpデータから開く
    void mocFileOpen(const mstring& filename,
                     AeroPhotoObjectList& aeroPhotoObjectList,
                     RenderMain& renderMain,
                     ShpFile& shp,
                     DBaseFile& dbase) {
      // データディレクトリの生成
      m_rootDirectory = util::getDirectoryName(filename);
      aeroPhotoObjectList.setDataDir(m_rootDirectory);
      TextureManager::SetDirectory(m_rootDirectory + "image/");

      // マップデータの読み込み
      try {
        ifstream ifs(filename.c_str(), ios_base::binary);
        aeroPhotoObjectList.read(ifs);
      }
      catch (const std::exception&) {
        aeroPhotoObjectList.clear();
      }
      for (ShpFile::iterator i = shp.getRecordData().begin(); i != shp.getRecordData().end(); ++i) {
        shared_ptr<ShpFile::RecordHeader> rec(*i);
        switch(rec->shapeType) {
        case ShpFile::PolygonType:
          {
            ShpFile::Polygon* pg = dynamic_cast<ShpFile::Polygon*>(rec.get());
            assert(pg != 0);

            // dbfファイルから画像ファイル名を取得してロードする
            uint32_t  index = rec->recordNumber - 1; // shpではレコードは1から始まる
            mstring imageName(dbase.get(index, "ImageName"));
            mstring imagePath(dbase.get(index, "ImagePath"));
            mstring imageExt(dbase.get(index, "ImageExt"));
            mstring shotOrient(dbase.get(index, "ShotOrient")); // 方向
            mstring shotType(dbase.get(index, "ShotType")); // 方向
            mstring cameraX(dbase.get(index, "CameraX")); // 方向
            mstring cameraY(dbase.get(index, "CameraY")); // 方向
            mstring cameraZ(dbase.get(index, "Alt")); // 方向
            mstring cameraOmega(dbase.get(index, "Omega")); // 方向
            mstring cameraPhi(dbase.get(index, "Phi")); // 方向
            mstring cameraKappa(dbase.get(index, "Kappa")); // 方向
            mstring cameraElevation(dbase.get(index, "Elevation")); // 標高
            imagePath.trim(" ");
            imageName.trim(" ");
            imageExt.trim(" ");
            mstring imageFn(imagePath.substr(3) + imageName + "." + imageExt);

            AeroPhotoObject::ptr mapObj = aeroPhotoObjectList.findAeroPhotoObject(imageName);
            if (mapObj.get() == 0) {
              // 見つからない場合は新規作成
              mapObj = AeroPhotoObject::ptr(new AeroPhotoObject);
              aeroPhotoObjectList.addAeroPhotoObject(mapObj);
              mapObj->setName(imageName);
            }
            mapObj->initialize(pg);
            // マップの方角を調べる
            if (shotType.trim(" ") == "Ob") {
              switch (shotOrient.trim(" ")[0]) {
              case 'N': mapObj->setDirection(MAPDIR_NORTH); break;
              case 'S': mapObj->setDirection(MAPDIR_SOUTH); break;
              case 'W': mapObj->setDirection(MAPDIR_WEST); break;
              case 'E': mapObj->setDirection(MAPDIR_EAST); break;
              default: break;
              }
            }
            else {
              mapObj->setDirection(MAPDIR_TOP);
            }

            mapObj->originalCamera().x = cameraX.toDouble();
            mapObj->originalCamera().y = cameraY.toDouble();
            mapObj->originalCamera().z = cameraZ.toDouble();
            mapObj->originalCameraAngle().x = cameraOmega.toDouble();
            mapObj->originalCameraAngle().y = cameraPhi.toDouble();
            mapObj->originalCameraAngle().z = cameraKappa.toDouble();
            mapObj->setOriginalCameraElevation(cameraElevation.toDouble());
  
            if (i == shp.getRecordData().begin()) {
              aeroPhotoObjectList.setCurrentAeroPhotoObject(mapObj->getId());
            }
            else {
              mapObj->setStatus(AeroPhotoObject::WireFrame);
            }
            mapObj->setImagePathName(m_rootDirectory + imageFn);
            mapObj->setZoffset(static_cast<float32_t>(rec->recordNumber)*4.0f);
            //uint32_t mapId =mapObj->getId();

            renderMain.getViewControl().addBoundary(mapObj->getVertex());
            renderMain.addGraphicsObject(mapObj->getGraphicsObject(), mapObj->getDirection());
            mapObj->setDbInfo(dbase.getRecord(index));
            //m_listDialog->setListData(mapId, mapObj);
          }
          break;
        default:
          ERRORLOG("unsupported shape %d\n", rec->shapeType);
          break;
        }
      }
    }
  };
}


#endif;