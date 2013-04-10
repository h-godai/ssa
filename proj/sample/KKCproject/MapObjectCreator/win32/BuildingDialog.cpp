// BuildingDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MapObjectCreatorWinApp.h"
#include "IUserInterfaceHandler.h"
#include "BuildingDialog.h"


// BuildingDialog ダイアログ

IMPLEMENT_DYNAMIC(BuildingDialog, CDialog)

BuildingDialog::BuildingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(BuildingDialog::IDD, pParent)
    ,   m_initialized(false)
{

}

BuildingDialog::~BuildingDialog()
{
}

void BuildingDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_BUILDING_PROPERTY, m_editProperty);
}


BEGIN_MESSAGE_MAP(BuildingDialog, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_ADD_BUILDING1, &BuildingDialog::OnBnClickedButtonAddBuilding1)
  ON_BN_CLICKED(IDC_BUTTON_ADD_BUILDING2, &BuildingDialog::OnBnClickedButtonAddBuilding2)
  ON_BN_CLICKED(IDC_BUTTON_ADD_BUILDING3, &BuildingDialog::OnBnClickedButtonAddBuilding3)
  ON_WM_SIZE()
  ON_WM_MOVE()
  ON_BN_CLICKED(IDC_BUTTON_SAVE_BUILDING, &BuildingDialog::OnBnClickedButtonSaveBuilding)
END_MESSAGE_MAP()


BOOL BuildingDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  ここに初期化を追加してください
  GetWindowRect(m_windowRect);
  int left   = theApp.GetProfileInt(REG_BUILDINGDIALOG, REG_LEFT, m_windowRect.left);
  int top    = theApp.GetProfileInt(REG_BUILDINGDIALOG, REG_TOP, m_windowRect.top);
  int width  = theApp.GetProfileInt(REG_BUILDINGDIALOG, REG_WIDTH, m_windowRect.Width());
  int height = theApp.GetProfileInt(REG_BUILDINGDIALOG, REG_HEIGHT, m_windowRect.Height());
  m_windowRect = CRect(left, top, left+width, top+height);
  MoveWindow(m_windowRect, FALSE);


  CButton* btn = static_cast<CButton*>(GetDlgItem(IDC_BUTTON_ADD_BUILDING1));
  if (btn != 0) {
    
    btn->SetIcon(AfxGetApp()->LoadIcon(IDC_BUTTON_ADD_BUILDING1));
  }
//  update(false);
  m_initialized = true;

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

// BuildingDialog メッセージ ハンドラ

void BuildingDialog::OnBnClickedButtonAddBuilding1()
{
  try {
    MainApp().onButtonAddBuilding(0);
  }
  catch(const std::exception&) {
    AfxMessageBox("なぜか失敗しました");
  }
}

void BuildingDialog::OnBnClickedButtonAddBuilding2()
{
  try {
    MainApp().onButtonAddBuilding(1);
  }
  catch(const std::exception&) {
    AfxMessageBox("なぜか失敗しました");
  }
}

void BuildingDialog::OnBnClickedButtonAddBuilding3()
{
  try {
    MainApp().onButtonAddBuilding(2);
  }
  catch(const std::exception&) {
    AfxMessageBox("なぜか失敗しました");
  }
}

void BuildingDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);
  if (m_initialized) {
     m_editProperty.MoveWindow(4, 128, cx-8, cy-60, TRUE);
    this->GetWindowRect(m_windowRect);
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_LEFT, m_windowRect.left);
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_TOP, m_windowRect.top);
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_WIDTH, m_windowRect.Width());
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_HEIGHT, m_windowRect.Height());
  }
}

void BuildingDialog::OnMove(int x, int y)
{
  CDialog::OnMove(x, y);
  if (m_initialized) {
    this->GetWindowRect(m_windowRect);
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_LEFT, m_windowRect.left);
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_TOP, m_windowRect.top);
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_WIDTH, m_windowRect.Width());
    theApp.WriteProfileInt(REG_BUILDINGDIALOG, REG_HEIGHT, m_windowRect.Height());
  }
}

void BuildingDialog::OnBnClickedButtonSaveBuilding()
{
  theApp.OnMenuSavebuildings();
}


