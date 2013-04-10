#pragma once


// EditTextureMenuBar ダイアログ

class EditTextureMenuBar : public CDialogBar
{
	DECLARE_DYNAMIC(EditTextureMenuBar)

public:
	EditTextureMenuBar(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~EditTextureMenuBar();
    void initialize();

// ダイアログ データ
	enum { IDD = IDR_EDITTEXTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
