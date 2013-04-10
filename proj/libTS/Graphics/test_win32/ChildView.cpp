// ChildView.cpp : CChildView クラスの実装
//

#include "stdafx.h"
#include "ChildView.h"
#include "libTS/Graphics/ext/OpenGLRenderer.h"
#include "libTS/Graphics/ext_win32/GraphicsContextWGL.h"
#include "libTS/Graphics/GraphicsObject.hpp"
#include "libTS/Image/Image.hpp"
#include "libTS/Image/extention/jpeg_image.hpp"
#include "test_win32.h"
#include "libTS/Util/ustring.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace boost;
using namespace ts;
using namespace ts::gobj;
// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
END_MESSAGE_MAP()



// CChildView メッセージ ハンドラ

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト
    static float th = 0.0f;
	
    //dc.FillSolidRect(CRect(0,0,100,100), (COLORREF)0x0080ff);
    CRect clientRect;
    GetClientRect(clientRect);

    if (m_wgl.get() != 0) {
      m_wgl->setClearColor(ColorRGBA(240,240,240,255));
      m_wgl->setHDC(dc.m_hDC);
      m_wgl->beginFrame();
      m_wgl->setViewPort(RectS32(static_cast<int32_t>(clientRect.left),
                                 static_cast<int32_t>(clientRect.top),
                                 static_cast<int32_t>(clientRect.Width()),
                                 static_cast<int32_t>(clientRect.Height())));

      ::glMatrixMode(GL_MODELVIEW);
      ::glLoadIdentity();

      float x = cos(th);
      float y = -sin(th);
      th += 3.241592f*2.0f/360.0f;


      m_gl->perspective(45.0f, static_cast<float32_t>(clientRect.Width())/clientRect.Height(), 0.1f, 10000.0f);
      m_gl->lookAt(Vector3D(0.0,0.0f,x*30.0f-60.0f), Vector3D(0.0f,0.0f,0.0f), Vector3D(x,y,0.0f));
      if (m_gobj) { 
          m_gobj->render(m_gl.get());
      }

      m_wgl->endFrame();
      
    }
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
  try {
    m_wgl = shared_ptr<ts::GraphicsContextWGL>(new GraphicsContextWGL(this->m_hWnd)); 
    m_gl = shared_ptr<OpenGLRenderer>(new OpenGLRenderer(m_wgl));
    Group::ptr group(new Group);
    Line3D::ptr line(new Line3D(Vector3D(-10.0f,-10.0f,10.0f),
                                      Vector3D(10.0f,10.0f,10.0f),
                                         ColorRGBA(0,0,0,255)));
    Rectangle3D::ptr rect3D(new Rectangle3D);
    Vector3D rectVtx[4] = {Vector3D(-32.0f, 24.0f,3.0f),
                           Vector3D( 32.0f, 24.0f,3.0f),
                           Vector3D( 32.0f,-24.0f,3.0f),
                           Vector3D(-32.0f,-24.0f,3.0f)};
    rect3D->setVertexes(rectVtx);

    image::basic_image<image::rgba8_pixel_t> buffer;
    try {
      image::basic_image<image::rgba8_pixel_t> jpeg;
      jpeg.loadFromFile(std::string("sample1.jpg"));
      jpeg.resizeTo(1024,1024,buffer);
    }
    catch (const image::exception& ) {
      // 読み込みエラー
      buffer.create(2,2);
      uint32_t* p = buffer.getBuffer<uint32_t*>();
      *p++ = 0xff0000ff;
      *p++ = 0x00ff00ff;
      *p++ = 0x0000ffff;
      *p++ = 0xffffffff;
    }

    Texture2D::ptr tex(new Texture2D);
    tex->setImageBuffer(buffer);
    rect3D->setTexture2D(tex);


    group->addChild(line);
    group->addChild(rect3D);
    m_gobj = group;
  }
  catch (const std::exception& ex) {
      ts::ustring msg(ex.what());
      AfxMessageBox(msg.getLPCTSTR());
  }
  catch(...) {
    assert(0);
  }
  SetTimer(100, 1000/60, NULL );
  return 0;
}

BOOL CChildView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
  // TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

  return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
  return TRUE;
//  return CWnd::OnEraseBkgnd(pDC);
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
  // TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

  RedrawWindow();
  CWnd::OnTimer(nIDEvent);
}
