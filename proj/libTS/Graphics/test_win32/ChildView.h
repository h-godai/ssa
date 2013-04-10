// ChildView.h : CChildView �N���X�̃C���^�[�t�F�C�X
//


#pragma once
#include "libTS/Graphics/ext/OpenGLRenderer.h"
#include "libTS/Graphics/ext_win32/GraphicsContextWGL.h"
#include "libTS/Graphics/GraphicsObject.hpp"
//#include "libTS/Graphics/forwards.h"

// CChildView �E�B���h�E

class CChildView : public CWnd
{
// �R���X�g���N�V����
public:
	CChildView();

// ����
public:
  boost::shared_ptr<ts::GraphicsContextWGL> m_wgl;
  boost::shared_ptr<ts::OpenGLRenderer> m_gl;
  ts::IGraphicsObject_ptr m_gobj;
  
          

// ����
public:

// �I�[�o�[���C�h
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~CChildView();

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
public:
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
};

