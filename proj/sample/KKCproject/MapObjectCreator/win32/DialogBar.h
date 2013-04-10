#pragma once


// DialogBar ダイアログ

class DialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(DialogBar)

public:
	DialogBar(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~DialogBar();

    void initialize();

// ダイアログ データ
	enum { IDD = IDR_MAINFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
