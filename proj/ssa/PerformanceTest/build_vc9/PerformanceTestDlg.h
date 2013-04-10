// PerformanceTestDlg.h : ヘッダー ファイル
//

#pragma once

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "PerformanceTestForm.h"
#include "afxwin.h"

// PerformanceTestDlg ダイアログ
class PerformanceTestDlg : public CDialog
{
// コンストラクション
public:
	PerformanceTestDlg(CWnd* pParent = NULL);	// 標準コンストラクタ
    ITestForm::ptr getTestForm() { return m_performanceTestForm; }

// ダイアログ データ
	enum { IDD = IDD_PERFORMANCETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

private:
  shared_ptr<PerformanceTestForm> m_performanceTestForm;

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton1();
  afx_msg void OnBnClickedButton2();
  afx_msg void OnBnClickedButton3();
  afx_msg void OnBnClickedButton4();
  afx_msg void OnBnClickedButton5();
  afx_msg void OnBnClickedButton6();
  CString m_result1;
  CString m_result2;
  CString m_result3;
  CString m_result4;
  CString m_result5;
  CString m_result6;
  CEdit m_console;
  CString m_outputText;
public:
  CComboBox m_kindComboBox;
  afx_msg void OnBnClickedButtonAll();
  afx_msg void OnCbnSelchangeComboKind();
  afx_msg void OnBnClickedButtonSave();
};
