// EditTextureMenuBar.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MapObjectCreatorWinApp.h"
#include "EditTextureMenuBar.h"
#include "IUserInterfaceHandler.h"


// EditTextureMenuBar ダイアログ

IMPLEMENT_DYNAMIC(EditTextureMenuBar, CDialogBar)

EditTextureMenuBar::EditTextureMenuBar(CWnd* /*pParent*/ /*=NULL*/)
	: CDialogBar()
{

}

EditTextureMenuBar::~EditTextureMenuBar()
{
}

void EditTextureMenuBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(EditTextureMenuBar, CDialogBar)
  ON_WM_CREATE()
END_MESSAGE_MAP()

int EditTextureMenuBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDialogBar::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  ここに特定な作成コードを追加してください。

  return 0;
}


// EditTextureMenuBar メッセージ ハンドラ


void EditTextureMenuBar::initialize() {
  CComboBox* cb = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_ZOOM));
  if (cb != 0) {
    cb->SetCurSel(3);
  }
}