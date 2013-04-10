// ChildView.h : ChildView �N���X�̃C���^�[�t�F�C�X
//


#pragma once
#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "Graphics/GraphicsObject.h"
#include "Graphics/ext_win32/GraphicsContextWGL.h"
#include "Graphics/ext/OpenGLRenderer.h"

using namespace app;
using boost::shared_ptr;
using namespace ts;

// ChildView �E�B���h�E

class ChildView : public CWnd
{
// �R���X�g���N�V����
public:
	ChildView();

// ����
public:
  shared_ptr<GraphicsContextWGL> m_wgl;
  shared_ptr<OpenGLRenderer> m_gl;


// ����
public:

// �I�[�o�[���C�h
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~ChildView();

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

