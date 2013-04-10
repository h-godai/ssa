// DXFrame.h : ヘッダー ファイル -*-C++-*-
//
//
// DirectX7 DirectDrawフレームサポートクラス required Class3dlib
//
// 2000.3.11 h.godai

#if !defined(AFX_DXFRAME_H__6AD0FC0A_66E9_4F9D_B2E4_24192EB8BBAC__INCLUDED_)
#define AFX_DXFRAME_H__6AD0FC0A_66E9_4F9D_B2E4_24192EB8BBAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __DX3DOBJECT_H__
#include "dx3dobject.h"
#endif

extern char* g3DODataDir;
extern bool gMouseMove;

class C3DObjectList;
class CResultDialog;

// CDXFrame用のオブジェクトクラス

extern CSize gResolution;			// 画面の解像度


/////////////////////////////////////////////////////////////////////////////
// CDXFrame フレーム

class CDXFrame : public CFrameWnd
{
  DECLARE_DYNCREATE(CDXFrame)    
     public:
  CDXFrame();           // 動的生成に使用されるプロテクト コンストラクタ。
  
// アトリビュート
public:
  C3DObjectList* m_p3DObjects;		// 3Dオブジェクト描画クラス
  CResultDialog* m_pResultDialog;	// ベンチマーク結果表示用のダイアログ
  int   m_SubViewHeight;		// サブ画面の高さ

  HIMC m_hImc;
  bool m_bImeOpen;

// オペレーション
public:
  
// オーバーライド
  // ClassWizard は仮想関数のオーバーライドを生成します。
  //{{AFX_VIRTUAL(CDXFrame)
	public:
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
  
// インプリメンテーション
public:
  virtual ~CDXFrame();
  
//protected:
  afx_msg LRESULT OnMyImeNotify(WPARAM dwCommand, LPARAM dwData);

  // 生成されたメッセージ マップ関数
  //{{AFX_MSG(CDXFrame)
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnClose();
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMove(int x, int y);
  afx_msg void OnPaint();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
  afx_msg void OnDestroy();
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnFileLoad();
  afx_msg void OnLoadmap();
  afx_msg void OnSavemap();
  afx_msg void OnEditMapitem();
  afx_msg void OnMenuResult();
  afx_msg void OnMenuInit();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEndInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTogglefullscreen();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
       
       
     private:
  LPCX3DFrame 	m_framework;
  LPCX3DViewport	m_viewport;
  LPCX3DCamera	m_camera;
  
  
  BOOL		m_bActive;
  BOOL		m_bReady;

  
  
public:
  bool	m_LButton;
  CPoint m_MousePoint;	// マウスの位置

  bool m_bInitMode;
  bool m_bWindowed;
  bool m_ForceWindowMode;
  
  D3DDEVICEINFO* m_pD3DDeviceInfo;
  
  // ベンチマーク用パラメータ
  DWORD m_BenchStartTime;
  DWORD m_BenchEndTime;
  int m_BenchFrames;
  int m_MinTime;
  int m_MaxTime;
  int m_BenchPolygons;
  int m_BenchVertexes ;
  int m_BenchWidth;
  int m_BenchHeight;
  int m_BenchDeps;
  float m_BestPolygons;
  float m_BestVertexes;
  
  
  void StartBench(DWORD curtime);
  void StopBench(DWORD curtime, bool fdlg=true);
  
  BOOL IsActive()
  {
    return m_bActive;
  }
  BOOL SetReady(BOOL fr)
  {
    BOOL	f = m_bReady;
    m_bReady = fr;
    return f;
  }


  BOOL	FInit(int nCmdShow);

  HRESULT Idle();
  HRESULT ChangeDirect3D();
  void ChangeCameraMode();
  HRESULT DeleteData();
  void	ShowStats(DWORD);
  void	OutputText(DWORD x, DWORD y, CHAR* str);
  void SetCamera();

  BOOL	OnCreate(HWND hwnd);
  void	OnDestroy(HWND hwnd);
  void	OnClose(HWND hwnd);
  void	OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
  void	OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
  void	OnKey(HWND hwnd, UINT vk, BOOL down, int repeat, UINT flags);
  void	OnMouseMove(HWND hwnd, int x, int y, UINT keyflags);
  void	OnPaint(HWND hwnd);
  void	OnMove(HWND hwnd, int x, int y);
  void	OnSize(HWND hwnd, UINT state, int cx, int cy);
  void	OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO mmi);
  BOOL	OnSetCursor(HWND hwnd, HWND hwndCursor, UINT hitCode, UINT msg);
  void	OnEnterMenuLoop(HWND hwnd, BOOL isTrackPopup);
  void	OnExitMenuLoop(HWND hwnd, BOOL isTrackPopup);
  BOOL	OnContextMenu(HWND hwnd, HWND hwndctl, int x, int y);
  void	OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
  void	OnActivate(HWND hwnd, UINT state, HWND hActive, BOOL minimized);


  void	RestoreObject();
  void	ReleaseObject();
  void	ResumeObject();

  void	app_open();
  void	app_close();
  void	app_drawstage();

  void SetFullScreenMode(bool fl=true, bool flagset=true);
  void MusicStop();
  void MoveCamera();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DXFRAME_H__6AD0FC0A_66E9_4F9D_B2E4_24192EB8BBAC__INCLUDED_)
