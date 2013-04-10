// MainFrm.h : MainFrame �N���X�̃C���^�[�t�F�C�X
//


#pragma once

#include "ChildView.h"
#include "DialogBar.h"
#include "EditTextureMenuBar.h"
#include "AppStatusManager.hpp"

class MainFrame : public CFrameWnd, private app::IAppStatusEventHandler
{
	
public:
	MainFrame();
protected: 
	DECLARE_DYNAMIC(MainFrame)

// ����
public:

// ����
public:

// �I�[�o�[���C�h
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// ����
public:
	virtual ~MainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    void setCameraPosString(const string& str, const string& target, const string& fovy);

    bool getChkImage2D() {
      CButton* btn = static_cast<CButton*>(m_wndDlgBar.GetDlgItem(IDC_CHECK_2DIMAGE));
      return btn->GetCheck() == TRUE;
    }
    bool getChkGrid() {
      CButton* btn = static_cast<CButton*>(m_wndDlgBar.GetDlgItem(IDC_CHECK_SHOWGRID));
      return btn->GetCheck() == TRUE;
    }
    bool getChkWireFrame() {
      CButton* btn = static_cast<CButton*>(m_wndDlgBar.GetDlgItem(IDC_CHECK_BUILDING_WIREFRAME));
      return btn->GetCheck() == TRUE;
    }
    bool getChkGroundMap() {
      CButton* btn = static_cast<CButton*>(m_wndDlgBar.GetDlgItem(IDC_CHECK_GROUNDMAP));
      return btn->GetCheck() == TRUE;
    }
    int getZoomRatio() {
      CComboBox* cb = static_cast<CComboBox*>(m_wndEditTexMenuBar.GetDlgItem(IDC_COMBO_ZOOM));
      if (cb != 0) return cb->GetCurSel();
      return -1;
    }

    void setStatusBarText(const std::string& text);

    void setCutBuidingText(const std::string& text);

    // StatusText���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
    virtual void onSetStatusText(const string& str);
    // AppMode���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
    virtual void onSetAppMode(AppMode );

protected:  // �R���g���[�� �o�[�p�����o
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	DialogBar      m_wndDlgBar;
    EditTextureMenuBar m_wndEditTexMenuBar;
	ChildView    m_wndView;
    CRect m_windowRect;
private:
  std::vector<std::string> m_statusText;

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMove(int x, int y);
  afx_msg void OnEnChangeEditCameraPos();
  afx_msg void OnBnClickedButtonCamera();
};


