// DialogBar.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MapObjectCreatorWinApp.h"
#include "DialogBar.h"
#include "IUserInterfaceHandler.h"

const int RoleSilderCenter = 512;
// DialogBar ダイアログ

IMPLEMENT_DYNAMIC(DialogBar, CDialogBar)

DialogBar::DialogBar(CWnd* /*pParent*/ /*=NULL*/)
	: CDialogBar()
{

}

DialogBar::~DialogBar()
{
}

void DialogBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogBar, CDialogBar)
  ON_WM_HSCROLL()
  ON_WM_CREATE()
END_MESSAGE_MAP()


// DialogBar メッセージ ハンドラ

void DialogBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION) {
    TRACE("HScroll: %x %d\n", nSBCode, nPos);
    if (theApp.getChildView() != 0) {
      MainApp().setRole(static_cast<float32_t>((static_cast<int>(nPos)-RoleSilderCenter)/(RoleSilderCenter*0.5)));
      MainApp().setRedrawRequest();
    }
  }

  CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}

int DialogBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDialogBar::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  ここに特定な作成コードを追加してください。

  return 0;
}

void DialogBar::initialize() {
  CSliderCtrl* slider = static_cast<CSliderCtrl*>(this->GetDlgItem(IDC_SLIDER_HEAD));
  slider->SetTic(RoleSilderCenter);
  slider->SetRange(0,RoleSilderCenter*2);
  slider->SetPos(RoleSilderCenter);
  slider->RedrawWindow();

  CButton* cb = static_cast<CButton*>(this->GetDlgItem(IDC_CHECK_2DIMAGE));
  cb->SetCheck(1);
  cb = static_cast<CButton*>(this->GetDlgItem(IDC_CHECK_SHOWGRID));
  cb->SetCheck(1);
  cb = static_cast<CButton*>(this->GetDlgItem(IDC_CHECK_GROUNDMAP));
  cb->SetCheck(1);
  
}

