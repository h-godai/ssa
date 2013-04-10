// ChildView.h : CChildView クラスのインターフェイス
//


#pragma once
#include "libTS/Graphics/ext/OpenGLRenderer.h"
#include "libTS/Graphics/ext_win32/GraphicsContextWGL.h"
#include "libTS/Graphics/GraphicsObject.hpp"
//#include "libTS/Graphics/forwards.h"

// CChildView ウィンドウ

class CChildView : public CWnd
{
// コンストラクション
public:
	CChildView();

// 属性
public:
  boost::shared_ptr<ts::GraphicsContextWGL> m_wgl;
  boost::shared_ptr<ts::OpenGLRenderer> m_gl;
  ts::IGraphicsObject_ptr m_gobj;
  
          

// 操作
public:

// オーバーライド
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 実装
public:
	virtual ~CChildView();

	// 生成された、メッセージ割り当て関数
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

