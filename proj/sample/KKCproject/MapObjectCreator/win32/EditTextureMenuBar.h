#pragma once


// EditTextureMenuBar �_�C�A���O

class EditTextureMenuBar : public CDialogBar
{
	DECLARE_DYNAMIC(EditTextureMenuBar)

public:
	EditTextureMenuBar(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~EditTextureMenuBar();
    void initialize();

// �_�C�A���O �f�[�^
	enum { IDD = IDR_EDITTEXTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
