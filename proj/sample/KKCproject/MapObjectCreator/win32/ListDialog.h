#pragma once
#include "afxcmn.h"
#ifndef STDAFX_H
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#endif
#include "AeroPhotoObject.hpp"

using boost::shared_ptr;
using namespace std;
using namespace app;

// ListDialog ダイアログ

class ListDialog : public CDialog
{
	DECLARE_DYNAMIC(ListDialog)

public:
	ListDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~ListDialog();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_LIST };


    typedef shared_ptr<vector<string> > RowDataType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  CListCtrl m_listView;
  virtual BOOL OnInitDialog();


  struct ColumnTitle {
    int width;
    string text;
    ColumnTitle(int w, const string& s) : width(w), text(s) {}
  };
  void clear() {
    m_listView.DeleteAllItems();
    m_columnTitle.clear();
    m_listData.clear();
  }

  void addColumnTitle(int w, const string& s) {
    m_columnTitle.push_back(ColumnTitle(w, s));
  }

  size_t getColumnCount() const {
    return m_columnTitle.size();
  }

  void update(bool redraw = true);

  void setListData(size_t row, AeroPhotoObject::ptr mapObj) {
    if (m_listData.size() <= row) {
      m_listData.resize(row+1);
    }
    m_listData[row] = mapObj;
  }

  afx_msg void OnLvnOdstatechangedListPic(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnMove(int x, int y);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnHdnItemclickListPic(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMClickListPic(NMHDR *pNMHDR, LRESULT *pResult);



private:
  vector<ColumnTitle> m_columnTitle;
  vector<AeroPhotoObject::ptr> m_listData;
  CRect m_windowRect;
  bool m_initialized;
public:
  afx_msg void OnDestroy();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
