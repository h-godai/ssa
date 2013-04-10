// ChildView.cpp : ChildView クラスの実装
//

#include "stdafx.h"
#include "MapObjectCreatorWinApp.h"
#include "ChildView.h"
#include "MainFrm.h"

#include "Graphics/ext/OpenGLRenderer.h"
#include "Graphics/ext_win32/GraphicsContextWGL.h"
#include "Graphics/GraphicsObject.h"
#include "Graphics/Image/ImageBuffer.hpp"
#include "Graphics/Image/JpegImage.hpp"
#include "Util/ustring.hpp"
#include "IUserInterfaceHandler.h"

using namespace ts;
using namespace ts::gobj;
using namespace app;
using boost::shared_ptr;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChildView

ChildView::ChildView()
{
}

ChildView::~ChildView()
{
}


BEGIN_MESSAGE_MAP(ChildView, CWnd)
	ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_ERASEBKGND()
//    ON_BN_CLICKED(IDC_CHECK_2DIMAGE, &ChildView::OnBnClickedCheck2dimage)
ON_WM_TIMER()
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// ChildView メッセージ ハンドラ

BOOL ChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}



void ChildView::OnPaint() 
{
  CPaintDC dc(this); // 描画のデバイス コンテキスト
  //dc.FillSolidRect(CRect(0,0,100,100), (COLORREF)0x0080ff);
  CRect clientRect;
  GetClientRect(clientRect);

  if (m_wgl.get() != 0) {
    m_wgl->setHDC(dc.m_hDC);
    RectS32 viewPort((int)clientRect.left, (int)clientRect.top, clientRect.Width(), clientRect.Height());

     MainApp().render(m_wgl, m_gl, viewPort);
  }
}



BOOL ChildView::OnEraseBkgnd(CDC* /*pDC*/)
{
  return TRUE;
}

BOOL ChildView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
  // TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

  BOOL result = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
  theApp.setChildView(this);

  m_wgl = shared_ptr<ts::GraphicsContextWGL>(new GraphicsContextWGL(this->m_hWnd)); 
  m_gl = shared_ptr<OpenGLRenderer>(new OpenGLRenderer(m_wgl));

  if (!MainApp().initialize()) {
    AfxMessageBox("初期化に失敗しました。");
    return FALSE;
  }
  SetTimer(1, 8, NULL); // 画面更新は約120Hz (8.34ms 16.67ms)

  // 以前の視点を設定する
  int x = theApp.GetProfileInt(REG_CAMERA, REG_TARGETX, 0);
  int y = theApp.GetProfileInt(REG_CAMERA, REG_TARGETY, 0);
  CameraPosition camera;
  camera.target.x = x;
  camera.target.y = y;
  camera.eye = camera.target;
  camera.eye.z = 1000.0;
  MainApp().setCameraPos(camera);

  return result;
}

MouseStatus& convWin32BtnFlag(MouseStatus& st, int x, int y, UINT nFlags, int zDelta = 0) {
  st.data = 0;
  st.x = x;
  st.y = y;
  st.dX = 0;
  st.dY = 0;
  st.wheel = zDelta;
  if ((nFlags & MK_CONTROL) != 0) st.btn.ctrlKey = 1;   // Ctrl キーが押されている場合に設定します。
  if ((nFlags & MK_LBUTTON) != 0) st.btn.leftBtn = 1;   // マウスの左ボタンが押されている場合に設定します。
  if ((nFlags & MK_MBUTTON) != 0) st.btn.centerBtn = 1; // マウスの中央ボタンが押されている場合に設定します。
  if ((nFlags & MK_RBUTTON) != 0) st.btn.rightBtn = 1;  // マウスの右ボタンが押されている場合に設定します。
  if ((nFlags & MK_SHIFT)   != 0) st.btn.shiftKey = 1;  // Shift キーが押されている場合に設定します。
  return st;
}
  

void ChildView::OnMouseMove(UINT nFlags, CPoint point)
{
  MouseStatus st;
  MainApp().onSetMousePosition(convWin32BtnFlag(st, point.x, point.y, nFlags));

  SetFocus();
//  CWnd::OnMouseMove(nFlags, point);
}

BOOL ChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  MouseStatus st;
  MainApp().onSetMousePosition(convWin32BtnFlag(st, pt.x, pt.y, nFlags, zDelta));

  return TRUE; //CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void ChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
  MouseStatus st;
  MainApp().onSetMousePosition(convWin32BtnFlag(st, point.x, point.y, nFlags));
  CWnd::OnLButtonDown(nFlags, point);
}

void ChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
  MouseStatus st;
  MainApp().onSetMousePosition(convWin32BtnFlag(st, point.x, point.y, nFlags));
  CWnd::OnLButtonUp(nFlags, point);
  // 視点の位置を保存しておく
  theApp.WriteProfileInt(REG_CAMERA, REG_TARGETX, static_cast<int>(MainApp().getCamera().target.x));
  theApp.WriteProfileInt(REG_CAMERA, REG_TARGETY, static_cast<int>(MainApp().getCamera().target.y));

}

void ChildView::OnTimer(UINT_PTR nIDEvent)
{

  CWnd::OnTimer(nIDEvent);
  MainApp().onTimer();
  if (MainApp().getRedrawRequest()) {
    mstring eyetxt, tgtxt, fovy;
    eyetxt.format("%.2f, %.2f, %.2f", 
      MainApp().getCamera().eye.x,
      MainApp().getCamera().eye.y,
      MainApp().getCamera().eye.z);
    tgtxt.format("%.2f, %.2f, %.2f", 
      MainApp().getCamera().target.x,
      MainApp().getCamera().target.y,
      MainApp().getCamera().target.z);
    fovy.format("%.3f (aspect:%.3f, role:%.3f)",
      MainApp().getCamera().fovy,
      MainApp().getAspect2D(),
      MainApp().getRole2D()
      );

    MainFrame* w = (MainFrame*)theApp.GetMainWnd();
    w->setCameraPosString(eyetxt, tgtxt, fovy);

    RedrawWindow();
  }

}

void ChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  try { 
    MainApp().onButtunDblClick();
  }
  catch(...) {
    AfxMessageBox("なぜか失敗しました");
  }

  CWnd::OnLButtonDblClk(nFlags, point);
}
