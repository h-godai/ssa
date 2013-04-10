// MapObjectCreatorWinApp.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#ifndef STDAFX_H
#include <fstream>
#endif

#include "MapObjectCreatorWinApp.h"
#include "MainFrm.h"
#include "ListDialog.h"
#include "IUserInterfaceHandler.h"

using namespace std;
using namespace ts;
using namespace ts::gobj;
using namespace app;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// MapObjectCreatorWinApp

BEGIN_MESSAGE_MAP(MapObjectCreatorWinApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &MapObjectCreatorWinApp::OnAppAbout)
    ON_COMMAND(ID_FILE_OPEN, &MapObjectCreatorWinApp::OnFileOpen)
    ON_BN_CLICKED(IDC_BUTTON_LIST, &MapObjectCreatorWinApp::OnBnClickedButtonList)
    ON_BN_CLICKED(IDC_BUTTON_GO_TOP, &MapObjectCreatorWinApp::OnBnClickedButtonGoTop)
    ON_BN_CLICKED(IDC_BUTTON_CALC_CAMERA, &MapObjectCreatorWinApp::OnBnClickedButtonCalcCamera)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_CAMERA_POS, &MapObjectCreatorWinApp::OnBnClickedButtonSaveCameraPos)
    ON_BN_CLICKED(IDC_CHECK_2DIMAGE, &MapObjectCreatorWinApp::OnBnClickedCheck2dimage)
    ON_BN_CLICKED(IDC_CHECK_SHOWGRID, &MapObjectCreatorWinApp::OnBnClickedCheckShowgrid)
    ON_BN_CLICKED(IDC_BUTTON_EDITOBJ, &MapObjectCreatorWinApp::OnBnClickedButtonEditobj)
    ON_BN_CLICKED(IDC_CHECK_BUILDING_WIREFRAME, &MapObjectCreatorWinApp::OnBnClickedCheckBuildingWireframe)
    ON_BN_CLICKED(IDC_BUTTON_VIEWMODE, &MapObjectCreatorWinApp::OnBnClickedButtonViewmode)
    //ON_BN_CLICKED(IDC_BUTTON_BULDINGLIST, &MapObjectCreatorWinApp::OnBnClickedButtonBuldinglist)
    ON_BN_CLICKED(IDC_CHECK_GROUNDMAP, &MapObjectCreatorWinApp::OnBnClickedCheckGroundmap)
    ON_BN_CLICKED(IDC_BUTTON_EDITTEXTURE, &MapObjectCreatorWinApp::OnBnClickedButtonEdittexture)
    ON_CBN_SELCHANGE(IDC_COMBO_ZOOM, &MapObjectCreatorWinApp::OnCbnSelchangeComboZoom)
    ON_BN_CLICKED(ID_EDIT_COPY, &MapObjectCreatorWinApp::OnBnClickedEditCopy)
    ON_BN_CLICKED(ID_EDIT_PASTE, &MapObjectCreatorWinApp::OnBnClickedEditPaste)
    ON_BN_CLICKED(IDC_BUTTON_RECALC, &MapObjectCreatorWinApp::OnBnClickedButtonRecalc)
    ON_COMMAND(ID_MENU_IMPORT_GLD, &MapObjectCreatorWinApp::OnMenuImportGld)
    ON_BN_CLICKED(IDC_BUTTON_MAKETEXTURE, &MapObjectCreatorWinApp::OnBnClickedMakeTexture)
    ON_BN_CLICKED(IDC_BUTTON_NORTH, &MapObjectCreatorWinApp::OnBnClickedButtonNorth)
    ON_BN_CLICKED(IDC_BUTTON_SOUTH, &MapObjectCreatorWinApp::OnBnClickedButtonSouth)
    ON_BN_CLICKED(IDC_BUTTON_WEST, &MapObjectCreatorWinApp::OnBnClickedButtonWest)
    ON_BN_CLICKED(IDC_BUTTON_EAST, &MapObjectCreatorWinApp::OnBnClickedButtonEast)
    ON_BN_CLICKED(IDC_BUTTON_IMPORT_CAMERA, &MapObjectCreatorWinApp::OnBnClickedImportCamera)
    ON_BN_CLICKED(IDC_BUTTON_TOPDOWN, &MapObjectCreatorWinApp::OnBnClickedButtonTopDown)   
    ON_COMMAND(ID_MENU_EXPORT_BUILDING, &MapObjectCreatorWinApp::OnMenuExportBuilding)
    ON_COMMAND(ID_MENU_SAVEBUILDINGS, &MapObjectCreatorWinApp::OnMenuSavebuildings)
    ON_BN_CLICKED(IDC_BUTTON_CUT_BULDING, &MapObjectCreatorWinApp::OnBnClickedButtonCutBulding)
    ON_BN_CLICKED(IDC_BUTTON_ADD_CUBE, &MapObjectCreatorWinApp::OnBnClickedButtonAddCube)
END_MESSAGE_MAP()


// MapObjectCreatorWinApp �R���X�g���N�V����

MapObjectCreatorWinApp::MapObjectCreatorWinApp()
: m_childView(0)
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� MapObjectCreatorWinApp �I�u�W�F�N�g�ł��B

MapObjectCreatorWinApp theApp;


// MapObjectCreatorWinApp ������

BOOL MapObjectCreatorWinApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	SetRegistryKey(_T("HS_REALITY"));
	// ���C�� �E�B���h�E���쐬����Ƃ��A���̃R�[�h�͐V�����t���[�� �E�B���h�E �I�u�W�F�N�g���쐬���A
	// ������A�v���P�[�V�����̃��C�� �E�B���h�E�ɃZ�b�g���܂�
	MainFrame* pFrame = new MainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// �t���[�������\�[�X���烍�[�h���č쐬���܂�
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);






	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s���܂��B
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// �ڔ��������݂���ꍇ�ɂ̂� DragAcceptFiles ���Ăяo���Ă��������B
	//  SDI �A�v���P�[�V�����ł́AProcessShellCommand �̒���ɂ��̌Ăяo�����������Ȃ���΂Ȃ�܂���B

  m_listDialog.Create(ListDialog::IDD, 0);
  m_listDialog.ShowWindow(SW_HIDE);
  m_buildingDialog.Create(BuildingDialog::IDD, 0);
  m_buildingDialog.ShowWindow(SW_HIDE);
	return TRUE;
}


// MapObjectCreatorWinApp ���b�Z�[�W �n���h��




// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �_�C�A���O�����s���邽�߂̃A�v���P�[�V���� �R�}���h
void MapObjectCreatorWinApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// MapObjectCreatorWinApp ���b�Z�[�W �n���h��

// shp�t�@�C�����J���B�Ō�ɓǂݍ��񂾃f�B���N�g�����o���Ă���
void MapObjectCreatorWinApp::OnFileOpen() {
  ts::mstring mfn = fileOpen(_T("all file(*.shp;*.bld;*.moc)|*.shp;*.bld;*.moc|SHP File (*.shp)|*.shp|Building File (*.bld)|*.bld|MapObjectFile (*.moc)|*.moc||"));
  if (mfn.empty()) return;
  try {
    if (mfn.rightMatch("bld")) {
      // �����t�@�C��
      MainApp().bldFileOpen(mfn);
    }
    else if (mfn.rightMatch("moc")) {
      // MOC�t�@�C��
      MainApp().mocFileOpen(mfn);
    }
    else {
      // shp�t�@�C��
      MainApp().shpFileOpen(mfn);
    }

    m_childView->RedrawWindow();
    m_listDialog.update();
  }
  catch (const std::exception& ex) {
    mstring msg;
    msg.format("�f�[�^�̓ǂݍ��݂ɂȂ������s���܂����B(%s)", ex.what());
    AfxMessageBox(msg.c_str(), MB_ICONQUESTION);
  }
}

ts::mstring MapObjectCreatorWinApp::fileOpen(const TCHAR* filter, bool read, const TCHAR* defaultFn) {
  const int bufferSize = 4096;
  TCHAR dirBuf[bufferSize];
  CString reg;
  ::GetCurrentDirectory(static_cast<DWORD>(bufferSize), dirBuf);


  CString fileName;
  if (read) {
    reg = this->GetProfileString(REG_DIRECTORY, REG_SHPDIR, dirBuf);
    CFileDialog dlgFile(TRUE, _T("moc"), NULL, 
                      OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_ENABLESIZING,
                      filter, NULL);
    dlgFile.GetOFN().lpstrInitialDir = reg;
    dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(bufferSize);
    dlgFile.GetOFN().nMaxFile = bufferSize;

    INT_PTR nResult = dlgFile.DoModal();
    if (nResult != IDOK) return ts::mstring();
  }
  else {
    reg = this->GetProfileString(REG_DIRECTORY, REG_EXPORTDIR, dirBuf);
    CFileDialog dlgFile(FALSE, _T("vrml"), defaultFn, 
                      OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_ENABLESIZING,
                      filter, NULL);
    dlgFile.GetOFN().lpstrInitialDir = reg;
    dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(bufferSize);
    dlgFile.GetOFN().nMaxFile = bufferSize;

    INT_PTR nResult = dlgFile.DoModal();
    if (nResult != IDOK) return ts::mstring();
  }


  TCHAR drive[4];
  TCHAR dir[bufferSize];
  TCHAR fn[bufferSize];
  TCHAR ext[256];
  ::_tsplitpath_s(fileName.GetBuffer(), drive, dir, fn, ext);
  CString newDir(drive);
  newDir += dir;
  if (read) {
    this->WriteProfileString(REG_DIRECTORY, REG_SHPDIR, newDir.GetBuffer());
  }
  else {
    this->WriteProfileString(REG_DIRECTORY, REG_EXPORTDIR, newDir.GetBuffer());
  }
  ts::mstring mfn(fileName);
  fileName.ReleaseBuffer();
  return mfn;
}

void MapObjectCreatorWinApp::OnBnClickedButtonList() {
  if (!::IsWindow(m_listDialog.m_hWnd)) {
    m_listDialog.Create(ListDialog::IDD, 0);
    m_listDialog.ShowWindow(SW_SHOW);
  }
  else {
    WINDOWINFO pwi;
    m_listDialog.GetWindowInfo(&pwi);
    if ((pwi.dwStyle & WS_VISIBLE) == 0) {
      m_listDialog.ShowWindow(SW_SHOW);
    }
    else {
      m_listDialog.ShowWindow(SW_HIDE);
    }
  }
}

void MapObjectCreatorWinApp::OnBnClickedButtonBuldinglist()
{
  if (!::IsWindow(m_buildingDialog.m_hWnd)) {
    m_buildingDialog.Create(ListDialog::IDD, 0);
    m_buildingDialog.ShowWindow(SW_SHOW);
  }
  else {
    WINDOWINFO pwi;
    m_buildingDialog.GetWindowInfo(&pwi);
    if ((pwi.dwStyle & WS_VISIBLE) == 0) {
      m_buildingDialog.ShowWindow(SW_SHOW);
    }
    else {
      m_buildingDialog.ShowWindow(SW_HIDE);
    }
  }
}


void MapObjectCreatorWinApp::OnBnClickedButtonGoTop()
{
  try {
    MainApp().onButtonGoTop();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

// �J�����ʒu�v�Z�{�^��
void MapObjectCreatorWinApp::OnBnClickedButtonCalcCamera()
{
  try {
    MainApp().onButtonCalcCamera();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedButtonSaveCameraPos()
{
  try {
    MainApp().onButtonSaveCameraPos();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }

}


void MapObjectCreatorWinApp::OnBnClickedCheck2dimage()
{
  MainApp().enableImage2D(static_cast<MainFrame*>(m_pMainWnd)->getChkImage2D());
}

void MapObjectCreatorWinApp::OnBnClickedCheckShowgrid()
{
  MainApp().enableGrid(static_cast<MainFrame*>(m_pMainWnd)->getChkGrid());
}

void MapObjectCreatorWinApp::OnBnClickedButtonEditobj() {
  try {
    MainApp().onButtonEditobj();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedCheckBuildingWireframe()
{
  MainApp().enableWireFrame(static_cast<MainFrame*>(m_pMainWnd)->getChkWireFrame());
}

void MapObjectCreatorWinApp::OnBnClickedButtonViewmode()
{
  try {
  MainApp().onButtonViewMode();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedCheckGroundmap()
{
  try {
  MainApp().enableGroundMap(static_cast<MainFrame*>(m_pMainWnd)->getChkGroundMap());
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedButtonEdittexture()
{
  try {
  MainApp().onButtonEditTextureMode();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnCbnSelchangeComboZoom()
{
  try {
  MainApp().onZoomChange(static_cast<MainFrame*>(m_pMainWnd)->getZoomRatio());
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedEditCopy()
{
  try {
    MainApp().onButtonCopy();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedEditPaste()
{
  try {
    MainApp().onButtonPaste();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedButtonRecalc()
{
  try {
    MainApp().onButtonRecalcCamera();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

void MapObjectCreatorWinApp::OnBnClickedMakeTexture() {
  try {
    MainApp().onMakeTexture();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }
}

#if 0
char** __argv;
int __argc;
#endif

void MapObjectCreatorWinApp::OnMenuImportGld()
{
  ts::mstring mfn = fileOpen(_T("gld file(*.txt)|*.txt|GLD File (*.txt)||"));
  try {
    if (mfn.rightMatch("txt")) {
      // GLD�t�@�C��
      MainApp().gldFileOpen(mfn);
    }
    m_childView->RedrawWindow();
  }
  catch (const std::exception& ex) {
    mstring msg;
    msg.format("�f�[�^�̓ǂݍ��݂ɂȂ������s���܂����B(%s)", ex.what());
    AfxMessageBox(msg.c_str(), MB_ICONQUESTION);
  }
}


namespace app {
  Vector3D gldOrigin(-6393.0f + (-6393 + 6693), -36231.0f + (-36231 + 35944), 0.0f);
}

void MapObjectCreatorWinApp::OnBnClickedButtonNorth()
{
  AppStatusManager::SetMapDirection(MAPDIR_NORTH);
}

void MapObjectCreatorWinApp::OnBnClickedButtonSouth()
{
  AppStatusManager::SetMapDirection(MAPDIR_SOUTH);
}

void MapObjectCreatorWinApp::OnBnClickedButtonWest()
{
  AppStatusManager::SetMapDirection(MAPDIR_WEST);
}

void MapObjectCreatorWinApp::OnBnClickedButtonEast()
{
  AppStatusManager::SetMapDirection(MAPDIR_EAST);
}

void MapObjectCreatorWinApp::OnBnClickedImportCamera() {
  try {
    MainApp().importCameraPos();
  }
  catch (const std::exception&) {
    AfxMessageBox(_T("�Ȃ������s���܂���"), MB_ICONQUESTION);
  }

}


void MapObjectCreatorWinApp::OnBnClickedButtonTopDown() {
  AppStatusManager::SetMapDirection(MAPDIR_TOP);
}



void MapObjectCreatorWinApp::OnMenuExportBuilding()
{
  ts::mstring mfn = fileOpen(_T("VRML file(*.vrml)|*.vrml|VRML File (*.vrml)||"), true, "Building.vrml");
  try {
    if (!mfn.empty()) {
      MainApp().exportBuldingToVrml(mfn);
    }
  }
  catch (const std::exception& ex) {
    mstring msg;
    msg.format("�f�[�^�̏����o���ɂȂ������s���܂����B(%s)", ex.what());
    AfxMessageBox(msg.c_str(), MB_ICONQUESTION);
  }
}


void MapObjectCreatorWinApp::OnMenuSavebuildings()
{
  ts::mstring mfn = fileOpen(_T("�����t�@�C��(*.bld)|*.bld|Building File (*.bld)||"), true, "Buildings.bld");
  try {
    if (!mfn.empty()) {
      MainApp().onButtonSaveBuilding(mfn);
    }
  }
  catch (const std::exception& ex) {
    mstring msg;
    msg.format("�f�[�^�̏����o���ɂȂ������s���܂����B(%s)", ex.what());
    AfxMessageBox(msg.c_str(), MB_ICONQUESTION);
  }
}

// �����ؒf�{�^��
void MapObjectCreatorWinApp::OnBnClickedButtonCutBulding()
{
  MainFrame* pFrame = static_cast<MainFrame*>(m_pMainWnd);
  if (MainApp().onButtonCutBuilding()) {
    pFrame->setCutBuidingText("�����̎��s");

  }
  else {
    pFrame->setCutBuidingText("�����̕���");
  }
}

void MapObjectCreatorWinApp::OnBnClickedButtonAddCube()
{
  if (!MainApp().onButtonAddCube()) {
    AfxMessageBox("�ǉ����錚����I�����Ă�������");
  }
}
