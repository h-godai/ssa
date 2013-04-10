// MapObjectCreatorWinApp.h : MapObjectCreatorWinApp アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "Resource.h"       // メイン シンボル
#include "ListDialog.h"
#include "BuildingDialog.h"
#include "Util/ustring.hpp"

using namespace app;
using std::vector;


using boost::shared_ptr;

// MapObjectCreatorWinApp:
// このクラスの実装については、MapObjectCreatorWinApp.cpp を参照してください。
//
class ChildView;
const TCHAR REG_DIRECTORY[]   = _T("Directory");
const TCHAR REG_SHPDIR[]      = _T("ShpFile");
const TCHAR REG_EXPORTDIR[]      = _T("Export");
const TCHAR REG_CAMERA[]      = _T("Camera");
const TCHAR REG_TARGETX[]      = _T("TargetX");
const TCHAR REG_TARGETY[]      = _T("TargetY");

const TCHAR REG_MAINWINDOW[]  = _T("MainWindow");
const TCHAR REG_LISTDIALOG[]  = _T("ListDialog");
const TCHAR REG_BUILDINGDIALOG[]  = _T("BuildingDialog");
const TCHAR REG_LEFT[]        = _T("left");
const TCHAR REG_TOP[]         = _T("top");
const TCHAR REG_WIDTH[]       = _T("width");
const TCHAR REG_HEIGHT[]      = _T("height");

class MapObjectCreatorWinApp : public CWinApp {
public:
  MapObjectCreatorWinApp();


// オーバーライド
public:
  virtual BOOL InitInstance();

  // 実装
  void setChildView(ChildView* v) { m_childView = v; }
  ChildView* getChildView() { return m_childView; }
  ts::mstring fileOpen(const TCHAR* filter, bool read=true, const TCHAR* =0);


public:
  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
  afx_msg void OnFileOpen();

private:
  ChildView* m_childView;
  ListDialog m_listDialog;
  BuildingDialog m_buildingDialog;
public:
  afx_msg void OnBnClickedButtonList();
  afx_msg void OnBnClickedButtonGoTop();
  afx_msg void OnBnClickedButtonCalcCamera();
  afx_msg void OnBnClickedButtonSaveCameraPos();
  afx_msg void OnBnClickedCheck2dimage();
  afx_msg void OnBnClickedCheckShowgrid();
  afx_msg void OnBnClickedButtonEditobj();
  afx_msg void OnBnClickedCheckBuildingWireframe();
  afx_msg void OnBnClickedButtonViewmode();
  afx_msg void OnBnClickedButtonBuldinglist();
  afx_msg void OnBnClickedCheckGroundmap();
  afx_msg void OnBnClickedButtonEdittexture();
  afx_msg void OnCbnSelchangeComboZoom();
  afx_msg void OnBnClickedEditCopy();
  afx_msg void OnBnClickedEditPaste();
  afx_msg void OnBnClickedButtonRecalc();
  afx_msg void OnMenuImportGld();
  afx_msg void OnBnClickedMakeTexture();
  afx_msg void OnBnClickedButtonNorth();
  afx_msg void OnBnClickedButtonSouth();
  afx_msg void OnBnClickedButtonWest();
  afx_msg void OnBnClickedButtonEast();
  afx_msg void OnBnClickedImportCamera();
  afx_msg void OnBnClickedButtonTopDown();
  
  afx_msg void OnMenuExportBuilding();
  afx_msg void OnMenuSavebuildings();
  afx_msg void OnBnClickedButtonCutBulding();
  afx_msg void OnBnClickedButtonAddCube();
};

extern MapObjectCreatorWinApp theApp;