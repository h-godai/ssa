#pragma once


// DialogBar �_�C�A���O

class DialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(DialogBar)

public:
	DialogBar(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~DialogBar();

    void initialize();

// �_�C�A���O �f�[�^
	enum { IDD = IDR_MAINFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
