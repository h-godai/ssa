// ListDialog.cpp : �����t�@�C��
//

#include "stdafx.h"
#ifndef STDAFX_H
#include <boost/foreach.hpp>
#endif
#include "MapObjectCreatorWinApp.h"
#include "ListDialog.h"
#include "IUserInterfaceHandler.h"


class MyListDialog : public app::IListDialog {
public:
  MyListDialog(ListDialog* listDialog)
    : m_listDialog(listDialog)
  {}
  virtual ~MyListDialog() {}
  virtual size_t getColumnCount() { return m_listDialog->getColumnCount(); }
  virtual void addColumnTitle(uint32_t length, const string& name) {
    m_listDialog->addColumnTitle(length, name);
  }
  virtual void update() { m_listDialog->update(); }
  virtual void setListData(uint32_t mapId, AeroPhotoObject::ptr mapObj) {
    m_listDialog->setListData(mapId, mapObj);
  }
private:
  ListDialog* m_listDialog;
};


// ListDialog �_�C�A���O

IMPLEMENT_DYNAMIC(ListDialog, CDialog)

ListDialog::ListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ListDialog::IDD, pParent)
    , m_initialized(false)
{

}

ListDialog::~ListDialog()
{
}

void ListDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_PIC, m_listView);
}


BEGIN_MESSAGE_MAP(ListDialog, CDialog)
  ON_NOTIFY(LVN_ODSTATECHANGED, IDC_LIST_PIC, &ListDialog::OnLvnOdstatechangedListPic)
  ON_WM_MOVE()
  ON_WM_SIZE()
  ON_NOTIFY(HDN_ITEMCLICK, 0, &ListDialog::OnHdnItemclickListPic)
  ON_NOTIFY(NM_CLICK, IDC_LIST_PIC, &ListDialog::OnNMClickListPic)
  ON_WM_DESTROY()
  ON_WM_CREATE()
END_MESSAGE_MAP()


// ListDialog ���b�Z�[�W �n���h��

int ListDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  �����ɓ���ȍ쐬�R�[�h��ǉ����Ă��������B

  return 0;
}


void ListDialog::OnDestroy()
{
  CDialog::OnDestroy();
  m_initialized = false;

}


BOOL ListDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  �����ɏ�������ǉ����Ă�������
  GetWindowRect(m_windowRect);
  int left   = theApp.GetProfileInt(REG_LISTDIALOG, REG_LEFT, m_windowRect.left);
  int top    = theApp.GetProfileInt(REG_LISTDIALOG, REG_TOP, m_windowRect.top);
  int width  = theApp.GetProfileInt(REG_LISTDIALOG, REG_WIDTH, m_windowRect.Width());
  int height = theApp.GetProfileInt(REG_LISTDIALOG, REG_HEIGHT, m_windowRect.Height());
  m_windowRect = CRect(left, top, left+width, top+height);
  MoveWindow(m_windowRect, FALSE);

  update(false);
  m_initialized = true;

  MainApp().setListDialog(IListDialog::ptr(new MyListDialog(this)));
  return TRUE;  // return TRUE unless you set the focus to a control
  // ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void ListDialog::update(bool redraw) {

  m_listView.DeleteAllItems();
  while (m_listView.DeleteColumn(0));

  int col = 0;
  int subitem = -1;

  BOOST_FOREACH(ColumnTitle& c, m_columnTitle) {
    m_listView.InsertColumn(++col, c.text.c_str(), LVCFMT_LEFT, c.width, subitem++);
    if (subitem == 0) subitem = 1;
  }

  // add
  BOOST_FOREACH(AeroPhotoObject::ptr& mapObj, m_listData) {
    if (mapObj.get() != 0) {
      DBaseFile::DataListPtr rowData = mapObj->getDbInfo();
      int pos = m_listView.GetItemCount();
      if (rowData.get() == 0) {
          m_listView.InsertItem(pos, mapObj->getName().c_str());
          m_listView.SetItemText(pos, 1, mapObj->getImagePath().c_str());
          m_listView.SetItemData(pos, static_cast<DWORD_PTR>(mapObj->getId()));
      }
      else {
          m_listView.InsertItem(pos, rowData->at(0).c_str());
          for (int n = 1; n < static_cast<int>(rowData->size()); ++n) {
            m_listView.SetItemText(pos, n, rowData->at(n).c_str());
          }
          m_listView.SetItemData(pos, static_cast<DWORD_PTR>(mapObj->getId()));
      }
    }
  }
  m_listView.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

  if (redraw) RedrawWindow();

}

void ListDialog::OnLvnOdstatechangedListPic(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
  //LPNMLVODSTATECHANGE pStateChanged = reinterpret_cast<LPNMLVODSTATECHANGE>(pNMHDR);
  // TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B


  *pResult = 0;
}

void ListDialog::OnMove(int x, int y)
{
  CDialog::OnMove(x, y);
  if (m_initialized) {
    this->GetWindowRect(m_windowRect);
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_LEFT, m_windowRect.left);
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_TOP, m_windowRect.top);
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_WIDTH, m_windowRect.Width());
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_HEIGHT, m_windowRect.Height());
  }
}

void ListDialog::OnSize(UINT nType, int cx, int cy)
{
  m_listView.MoveWindow(4, 64, cx-8, cy-60, TRUE);
  CDialog::OnSize(nType, cx, cy);
  if (m_initialized) {
    this->GetWindowRect(m_windowRect);
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_LEFT, m_windowRect.left);
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_TOP, m_windowRect.top);
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_WIDTH, m_windowRect.Width());
    theApp.WriteProfileInt(REG_LISTDIALOG, REG_HEIGHT, m_windowRect.Height());
  }
}

void ListDialog::OnHdnItemclickListPic(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
//  LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
  // TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
  *pResult = 0;
}

void ListDialog::OnNMClickListPic(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
  HCURSOR wcur = LoadCursor(NULL,IDC_WAIT);
  HCURSOR cur = SetCursor(wcur);
  // TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
  for (int row = 0; row < m_listView.GetItemCount(); ++row) {
    uint32_t mapId = static_cast<uint32_t>(m_listView.GetItemData(row));
    if (m_listView.GetItemState(row, LVIS_FOCUSED) != 0) {
      // �t�H�[�J�X��������̂��J�����g
      MainApp().setCurrentAeroPhotoObject(mapId);
    }
    if (m_listView.GetItemState(row, LVIS_SELECTED) != 0) {
      MainApp().setAeroPhotoObjectVisible(mapId);
    }
    else {
      MainApp().setAeroPhotoObjectWireFrame(mapId);
    }
  }
  SetCursor(cur);
  theApp.GetMainWnd()->RedrawWindow();
  *pResult = 0;
}
