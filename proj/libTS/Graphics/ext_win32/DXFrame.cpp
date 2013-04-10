// DXFrame.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "resource.h"
#include "class3dlib.h"


#include "SLSim.h"
#include "DXFrame.h"

#include "resultdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



int gTotalVertexes=0;
int gTotalPolygons=0;

bool gCanColorKeyAlpha    = false;   // �J���[�L�[�A���t�@�u�����h���g����t���O

bool gBiLinearEnable      = true;    // �o�C���j�A�t�B���^
bool gAlphaBrendingEnable = true;    // �A���t�@�u�����f�B���O�L��
bool gSpecularEnable	  = true;    // �X�y�L�����[�L��
bool gDitherEnable        = true;    // �f�B�U�L��
bool gAntiAliasEnable     = false;   // �A���`�G�C���A�X�L��
bool gTableFogEnable      = false;   // TABLE�t�H�O
bool gVertexFogEnable     = true;    // ���_�t�H�O
extern float gSikai;  //  = 300.0f;  // ���E
bool gShowStatusEnable = false;	     // ���\��
bool gShowDebugStatus = false;	     // ���\��
int gLightMax = 2;
extern int gSmokeVMAX;		     // ���̃|���S����

CSize gResolution = CSize(1024,600); // Window�̃f�t�H���g�T�C�Y


#define FRAMECLASS	"TECHARTS_SAMPLE"


DWORD gCurrentTime = 0;
DWORD gPastTime = 0;



C3DVector gCameraFrom = C3DVector(20.0f, -20.0f, 10.0f);
C3DVector gCameraTo = C3DVector(0.0f, 0.0f, 0.0f);
float gHDirectionTh=0.0f; // �����̐���������]
float gVDirectionTh=0.0f; // �����̐���������]


bool gAutoTurn = true;
bool gMouseMove = true;


HWND		_hWndFrame = NULL;

static BOOL Frame_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
  CDXFrame* pP;

  pP = (CDXFrame*)(lpCreateStruct->lpCreateParams);
  SetWindowLong( hwnd, 0, (LONG)(LPSTR)pP);

  _hWndFrame = hwnd;
  return pP->OnCreate(hwnd);		//����I��
}

long CALLBACK FullScreenWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  CDXFrame*	pP;
  pP = (CDXFrame*)GetWindowLong(hwnd, 0);

  switch(message) {
    HANDLE_MSG( hwnd, WM_CREATE,			Frame_OnCreate);
    HANDLE_MSG( hwnd, WM_KEYDOWN,			pP->OnKey);
    HANDLE_MSG( hwnd, WM_CLOSE, 			pP->OnClose);
    HANDLE_MSG( hwnd, WM_COMMAND,			pP->OnCommand);
    HANDLE_MSG( hwnd, WM_SETCURSOR, 		pP->OnSetCursor);
    HANDLE_MSG( hwnd, WM_GETMINMAXINFO, 	pP->OnGetMinMaxInfo);
    HANDLE_MSG( hwnd, WM_SIZE,				pP->OnSize);
    HANDLE_MSG( hwnd, WM_MOVE,				pP->OnMove);
    HANDLE_MSG( hwnd, WM_PAINT, 			pP->OnPaint);
    HANDLE_MSG( hwnd, WM_MOUSEMOVE, 		pP->OnMouseMove);
    HANDLE_MSG( hwnd, WM_RBUTTONDOWN,		pP->OnRButtonDown);
    HANDLE_MSG( hwnd, WM_LBUTTONDOWN,		pP->OnLButtonDown);
    HANDLE_MSG( hwnd, WM_ACTIVATE,			pP->OnActivate);
    HANDLE_MSG( hwnd, WM_DESTROY,			pP->OnDestroy);
  }

  return DefWindowProc( hwnd, message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CDXFrame

IMPLEMENT_DYNCREATE(CDXFrame, CFrameWnd)

CDXFrame::CDXFrame()
: m_p3DObjects(NULL),
  m_bInitMode(true),
  m_BenchWidth(0),
  m_BenchHeight(0),
  m_BenchDeps(0),
  m_SubViewHeight(0),
  m_bImeOpen(false)

{
 m_BenchStartTime = 0;
 m_BenchFrames = 0;
 m_MinTime = 10 * 1000;
 m_MaxTime = 1;
 m_BenchPolygons = 0;
 m_BenchVertexes = 0 ;
 m_BenchEndTime = 1;
 m_BestPolygons = 0.0;
 m_BestVertexes = 0.0;


  m_framework = NULL;
  m_bReady = FALSE;
  m_bActive = FALSE;

 m_ForceWindowMode = m_bWindowed = true;
	
  m_viewport = NULL;
  m_camera = NULL;

 m_pResultDialog = new CResultDialog(this);

 m_hImc = ::ImmCreateContext();

}

CDXFrame::~CDXFrame()
{
  delete m_pResultDialog;

  if(m_hWnd != NULL)
    {
      // CG: ���̍s�� �uIME �T�|�[�g�v�R���|�[�l���g�ɂ��ǉ�����Ă��܂��B
      ::ImmAssociateContext(m_hWnd, NULL);
      ::ImmDestroyContext(m_hImc);
    }
}



BOOL	CDXFrame::FInit(int nCmdShow)
{
//  ShowWindow( nCmdShow );
//  UpdateWindow();

  if (!D3DDeviceListup_Initialize()) 
    {
      AfxMessageBox(IDS_STRING_NODIRECT3D, MB_ICONERROR|MB_OK );
      DestroyWindow();
      return FALSE;
    }

  // �܂��f�t�H���g�ݒ�Ńt���[�����[�N�����

  D3DDEVICEINFO	* drvinfo;
  HRESULT			hr;
  hr = D3DDeviceListup_SelectDefaultDevice(&drvinfo);
  m_pD3DDeviceInfo = drvinfo;
  if (FAILED(hr)) {
    dbgprintfx("DeviceError!�K�؂ȃf�o�C�X���Ȃ�");
    return FALSE;
  }
  
  m_framework = new CX3DFrame();
  if (FAILED(m_framework->FInit(m_hWnd,
				drvinfo->pDriverGUID,
				drvinfo->pDeviceGUID, 
				&drvinfo->ddsdFullscreenMode,
				!m_bWindowed))) {
    SAFE_DELETE( m_framework );
    AfxMessageBox(IDS_STRING_NOFRAME, MB_ICONERROR|MB_OK );
    DestroyWindow();
    return FALSE;
  }

  // ViewPort�ƃJ�������쐬
  CRect rc;
  GetClientRect(rc);

  m_viewport = new CX3DViewport();
  m_viewport->FromWindow(NULL, m_hWnd);
  m_camera = new CX3DCamera();
  m_camera->SetViewFrom(DBL2D3DVECTOR(gCameraFrom));
  m_camera->FInit();
  int dy = _XDraw->DY();
  int dx = _XDraw->DX();
  m_camera->CalcProjectionMatrixVP(dx, dy-m_SubViewHeight);
  gResolution.cx = dx;
  gResolution.cy = dy;


  //---------------------------------------------------------
  
  if(!m_p3DObjects->Initialize())
    { // �������Ɏ��s������I��
      PostMessage(WM_CLOSE);
    }

  return TRUE;
}



//Direct3D������̕ύX
//
//
//
HRESULT	CDXFrame::ChangeDirect3D()
{
  static DWORD	dwStyle = 0;
  static DWORD	dwStyleParent = 0;
  static RECT		rcWindow;
  HRESULT hr;

//  if (!m_framework) return S_OK;


  if(!m_bWindowed) 
    {
      // Window�̏�Ԃ��擾
      dwStyle = GetWindowLong(m_hWnd, GWL_STYLE );
      GetWindowRect(&rcWindow );

      // ����Window���ړ�����
      // this->MoveWindow(0,0,800,600, FALSE);
    }


  if(m_framework)
    {
      ReleaseObject();
      delete m_framework;
      m_framework = NULL;
    }

  D3DDEVICEINFO	* drvinfo = m_pD3DDeviceInfo;
  //hr = D3DDeviceListup_SelectDefaultDevice(&drvinfo);
  
  
  if(!m_bWindowed) 
    { //�t���X�N���[���֕ύX
      if(drvinfo->ddsdFullscreenMode.dwWidth < 640 ||
	drvinfo->ddsdFullscreenMode.dwHeight < 480)
	{
	  AfxMessageBox("800x600dit�ȉ��̉𑜓x�ł͑���p�l���͐������\������܂���");
	}


      SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
    } 
  else if (dwStyle != 0) 
    {

      //�E�B���h�E�𕜋A
      SetWindowLong(m_hWnd, GWL_STYLE, dwStyle );
      //SetWindowLong(pwh, GWL_STYLE, dwStyleParent );
      ::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rcWindow.left, rcWindow.top,
		   ( rcWindow.right - rcWindow.left ), 
		   ( rcWindow.bottom - rcWindow.top ), SWP_SHOWWINDOW );
      //�E�B���h�E��Ԃő����ČĂяo����Ă��A�E�B���h�E�T�C�Y��ύX���Ȃ�
      dwStyle = 0;
    }


  //
  m_framework = new CX3DFrame();
  if (FAILED(hr = m_framework->FInit(m_hWnd,
				     drvinfo->pDriverGUID,
				     drvinfo->pDeviceGUID, 
				     &drvinfo->ddsdFullscreenMode,
				     !m_bWindowed))) {
    m_bReady = FALSE;
    AfxMessageBox(IDS_STRING_NOFRAME, MB_ICONERROR|MB_OK );
    DestroyWindow();
    return hr;
  }
  
  //  VirePort������������J�������ꏏ�ɍ��
  if(m_viewport == NULL)
    {
      m_viewport = new CX3DViewport();
      m_viewport->FromWindow(NULL, m_hWnd);
      
      m_camera = new CX3DCamera();
      m_camera->SetViewFrom(DBL2D3DVECTOR(gCameraFrom));
      m_camera->FInit();
    }

  if(m_bWindowed)
    { // Windows���[�h�ɂȂ���
      CRect rc;
      GetClientRect(rc);
      gResolution.cx = rc.Width();
      gResolution.cy = rc.Height();
      //m_viewport->FromWindow( m_camera, m_hWnd);
      //m_viewport(m_camera, 0,0, rc.right, rc.bottom);
      m_viewport->FInit(m_camera, 0,0, rc.right, rc.bottom - m_SubViewHeight);
      m_camera->CalcProjectionMatrixVP(rc.Width(), rc.Height()-m_SubViewHeight);

      ShowCursor(TRUE);

    }
  else
    { // FullScreen���[�h�ɂȂ���
      m_viewport->FInit(m_camera, 0,0,
			drvinfo->ddsdFullscreenMode.dwWidth,
			drvinfo->ddsdFullscreenMode.dwHeight - m_SubViewHeight);
      m_camera->CalcProjectionMatrixVP(
			drvinfo->ddsdFullscreenMode.dwWidth,
			drvinfo->ddsdFullscreenMode.dwHeight - m_SubViewHeight);

      gResolution.cx = drvinfo->ddsdFullscreenMode.dwWidth;
      gResolution.cy = drvinfo->ddsdFullscreenMode.dwHeight;



      // �J�[�\�����E��ɒǂ����
//    ::SetCursorPos(drvinfo->ddsdFullscreenMode.dwWidth, 0);
    }


  ResumeObject();
  
  return S_OK;
}


void CDXFrame::ChangeCameraMode()
{
  D3DDEVICEINFO	* drvinfo = m_pD3DDeviceInfo;

  if(m_bWindowed)
    { // Windows���[�h
      CRect rc;
      GetClientRect(rc);
      m_viewport->FInit(m_camera, 0,0, rc.right, rc.bottom - m_SubViewHeight);
      m_camera->CalcProjectionMatrixVP(rc.Width(), rc.Height()-m_SubViewHeight);

    }
  else
    { // FullScreen���[�h
      m_viewport->FInit(m_camera, 0,0,
			drvinfo->ddsdFullscreenMode.dwWidth,
			drvinfo->ddsdFullscreenMode.dwHeight - m_SubViewHeight);
      m_camera->CalcProjectionMatrixVP(
			drvinfo->ddsdFullscreenMode.dwWidth,
			drvinfo->ddsdFullscreenMode.dwHeight - m_SubViewHeight);
    }
}




HRESULT CDXFrame::DeleteData()
{
	m_bReady = FALSE;
	m_bActive = FALSE;

	ShowCursor( TRUE );

	SAFE_DELETE(m_p3DObjects);

	SAFE_DELETE( m_camera );
	SAFE_DELETE( m_viewport );
	SAFE_DELETE( m_framework );

	return S_OK;
}



//�A�v���P�[�V�����̂Ńo�b�O�X�e�[�^�X�\��
//
//
float gFPS = 0.0f;
float gAveFps = 0.0f;
int gPolygonsPerSec = 0;


void  CDXFrame::ShowStats(DWORD curtime)
{
  static DWORD last_time	= 0;
  static DWORD beforetime = 0;
  static DWORD frame		= 0;
  extern int gLightMode;
	
  DWORD diftime = curtime - last_time;
  DWORD pasttime = curtime - beforetime;
  beforetime = curtime;
  if(pasttime < 3) return;

  frame++;

  // fps�̌v�Z
  if(diftime > 200 && diftime != 0)
    { // 1/5�b�ȏ�o�߂�����FPS���v�Z
      gFPS   = (float)frame*1000.0f / (float)diftime;
      last_time = curtime;
      frame  = 0;
    }

  DDSURFACEDESC2 ddsd;
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
  _XDraw->BACK()->SURFACE()->GetSurfaceDesc(&ddsd);
  
  float pdif = 1000.0*gTotalPolygons  / (float)pasttime;
  float vdif = 1000.0*gTotalVertexes  / (float)pasttime;


  if(m_BenchEndTime == 0)
    { // �v����
      if(diftime < 5000 && diftime > m_MaxTime) m_MaxTime = diftime;
      if(diftime < m_MinTime) m_MinTime = diftime;
      if(pdif > m_BestPolygons) m_BestPolygons = pdif;
      if(vdif > m_BestVertexes) m_BestVertexes = vdif;
      
      m_BenchPolygons += gTotalPolygons;
      m_BenchVertexes += gTotalVertexes;
      ++m_BenchFrames;
      
      m_BenchWidth = ddsd.dwWidth;
      m_BenchHeight = ddsd.dwHeight;
      m_BenchDeps = ddsd.ddpfPixelFormat.dwRGBBitCount;

	    


    }

  gPolygonsPerSec = int(pdif);
  float btm = (curtime - m_BenchStartTime) / 1000.0;
  gAveFps =  m_BenchFrames / btm;

  if(!gShowDebugStatus) return;

  char buffer[160];
  sprintf( buffer, 
	   "%7.3f Frames/Sec, %d Vertexes, %d Polygons, %d Polygons/Sec (%dx%dx%d)",
	   gFPS, 
	   gTotalVertexes, gTotalPolygons, int(pdif),
	   ddsd.dwWidth, 
	   ddsd.dwHeight, 
	   ddsd.ddpfPixelFormat.dwRGBBitCount);

  OutputText( 0, 32, buffer );
}



//�A�v���P�[�V�����̉�ʂɃe�L�X�g��\������
//
//�f�o�b�O�X�e�[�^�X�p
void	CDXFrame::OutputText(DWORD x, DWORD y, CHAR* str )
{
	LPDIRECTDRAWSURFACE7 pddsRenderSurface;
	if (FAILED(_Direct3DDevice->GetRenderTarget( &pddsRenderSurface )))
		return;

	HDC hDC;
	if (SUCCEEDED(pddsRenderSurface->GetDC(&hDC))) {
		SetTextColor( hDC, RGB(255,255,255) );
		SetBkMode( hDC, TRANSPARENT );
		ExtTextOut( hDC, x, y, 0, NULL, str, strlen(str), NULL );
	
		pddsRenderSurface->ReleaseDC(hDC);
	}
	pddsRenderSurface->Release();
}



//�T�[�t�F�[�X�č\�z���s
//
//
//
void	CDXFrame::RestoreObject()
{
  m_p3DObjects->Restore();


}

//�T�[�t�F�[�X�I�u�W�F�N�g�̈ꎞ��~
//
//	�e�N�X�`���[�Ȃ�VRAM�Ɋm�ۂ���Ă���f�[�^��Release
void	CDXFrame::ReleaseObject()
{
  m_p3DObjects->Release();

}


//�T�[�t�F�[�X�I�u�W�F�N�g�̍ĊJ
//
//	�e�N�X�`���[�Ȃ�VRAM�Ɋm�ۂ���Ă���f�[�^��Resume
void CDXFrame::ResumeObject()
{
  m_p3DObjects->Resume();
}




//=====================================================





//�A�C�h�����O
//
//
//
HRESULT	CDXFrame::Idle()
{
  static int waitcnt = 0;



  //�܂����s�󋵂������Ă��Ȃ�
  if (!m_bActive || !m_bReady || m_bInitMode) return S_OK;
  if (!_Direct2D) return S_OK;
  if (FAILED(_Direct2D->TestCooperativeLevel())) return S_OK;

  if(GetAsyncKeyState(VK_MENU) < 0 && GetAsyncKeyState(VK_RETURN) < 0)
    {
      OnTogglefullscreen();
      return S_OK;
    }



  if(m_ForceWindowMode != m_bWindowed)
    {
      SetFullScreenMode(!m_ForceWindowMode);
      if(m_ForceWindowMode == m_bWindowed)
	{ // ���������班���܂�
	  //waitcnt = 30;
	}
    }

  if(waitcnt > 0)
    {
      --waitcnt;
      return S_OK;
    }


    // ���ݎ����ƌo�ߎ��Ԃ�ݒ�
  gCurrentTime = timeGetTime();
  static DWORD oldtime = 0;
  gPastTime = gCurrentTime - oldtime;
  oldtime = gCurrentTime;
  if(gPastTime > 10000) gPastTime = 0; // 10�b�ȏ�Ń��Z�b�g

  if(gMouseMove)
    MoveCamera();
  
  m_camera->Active();

  // �I�u�W�F�N�g�̕`����
  m_p3DObjects->Draw(gCurrentTime);

  
  
  if(_XDraw->Flip() == DDERR_SURFACELOST) 
    {
      m_framework->RestoreSurfaces();
      RestoreObject();
    }

  return S_OK;
}






BEGIN_MESSAGE_MAP(CDXFrame, CFrameWnd)
	ON_MESSAGE(WM_IME_NOTIFY, OnMyImeNotify)
	//{{AFX_MSG_MAP(CDXFrame)
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
     ON_COMMAND(ID_LOADMAP, OnLoadmap)
	ON_COMMAND(ID_SAVEMAP, OnSavemap)
	ON_COMMAND(ID_EDIT_MAPITEM, OnEditMapitem)
	ON_COMMAND(ID_MENU_RESULT, OnMenuResult)
	ON_COMMAND(ID_MENU_INIT, OnMenuInit)
	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDM_ENDDIALOG, OnEndInitDialog)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(IDM_TOGGLEFULLSCREEN, OnTogglefullscreen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDXFrame ���b�Z�[�W �n���h��




BOOL	CDXFrame::OnCreate(HWND hwnd)
{
	m_hWnd = hwnd;
	return TRUE;
}

BOOL CDXFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
  return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}




void	CDXFrame::OnDestroy(HWND hwnd) 
{
  DeleteData();
  PostQuitMessage( 0 );
}

void CDXFrame::OnDestroy() 
{
  DeleteData();
  CFrameWnd::OnDestroy();
}



void	CDXFrame::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyflags)
{
  m_LButton = true;
  m_p3DObjects->OnLButtonDown(keyflags, CPoint(x,y));

}


void	CDXFrame::OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
}

void CDXFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
  OnRButtonDown(m_hWnd, FALSE, point.x, point.y, nFlags);

  if(m_bWindowed)
    CFrameWnd::OnRButtonDown(nFlags, point);
}

void CDXFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
  OnLButtonDown(m_hWnd, FALSE, point.x, point.y, nFlags);
  if(m_bWindowed)
    CFrameWnd::OnLButtonDown(nFlags, point);
}


void CDXFrame::OnMouseMove(HWND hwnd, int x, int y, UINT keyflags)
{
  if(keyflags & MK_LBUTTON)
    m_LButton = true;
  else
    m_LButton = false;

  if(!m_bWindowed)
    { //�t���X�N���[����������
      CPoint po;
      GetCursorPos(&po);
      x = po.x;
      y = po.y;
    }

  m_MousePoint = CPoint(x,y);

  if(!gMouseMove) return;


  static int ox = 0;
  static int oy = 0;
  static double dis = 15.0; //�J�������王�_�܂ł̋���

  double dx = gCameraFrom.x - gCameraTo.x;
  double dy = gCameraFrom.y - gCameraTo.y;
  double dz = gCameraFrom.z - gCameraTo.z;
  double k = sqrt(1.0/(dx*dx+dy*dy+dz*dz));
  dx *= -k;
  dy *= -k;
  dz *= -k;

  if((keyflags & (MK_MBUTTON | MK_SHIFT | MK_LBUTTON | MK_RBUTTON)))
    {
      gCameraTo.x = gCameraFrom.x + dx*10.0;
      gCameraTo.y = gCameraFrom.y + dy*10.0;
      gCameraTo.z = gCameraFrom.z + dz*10.0;
    }

  double xyth = (ox - x) / 10.0 * PI / 180.0;
  double xx = dx * cos(xyth) + dy * sin(xyth);
  double yy = dy * cos(xyth) - dx * sin(xyth);
      
  // �ψꉻ
  if((keyflags & MK_MBUTTON) != 0 || (keyflags & MK_SHIFT) != 0)
    { // �^���̃{�^���̏ꍇ�͕��s�ړ�
      double kx = (x - ox) / 30.0;
      double ky = (y - oy) / 20.0;
      gCameraFrom.x -= dy*kx;
      gCameraFrom.y += dx*kx;
      gCameraFrom.z -= ky;
      gCameraTo.x -= dy*kx;
      gCameraTo.y += dx*kx;
      gCameraTo.z -= ky;


    }
  else
#if 0
  if(keyflags & MK_LBUTTON)
    { // L�{�^���������Ȃ��炾�ƁA�}�E�X�̏㉺�Ŏ��_�Ƃ̋�����ς���
      double kk = (oy - y) / 10.0;
      dx *= kk;
      dy *= kk;
      dz *= kk;
      gCameraFrom.x += dx;
      gCameraFrom.y += dy;
      gCameraFrom.z += dz;
      gCameraTo.x += dx;
      gCameraTo.y += dy;
      gCameraTo.z += dz;

      // �}�E�X�̍��E�Ŏ��_�����ɃJ��������]������
      dx = gCameraTo.x - gCameraFrom.x;
      dy = gCameraTo.y - gCameraFrom.y;
      dz = gCameraTo.z - gCameraFrom.z;
      double k = sqrt(1.0/(dx*dx+dy*dy+dz*dz));
      dx *= -k;
      dy *= -k;
      xyth = (ox - x) / 10.0 * PI / 180.0;
      xx = dx * cos(xyth) + dy * sin(xyth);
      yy = dy * cos(xyth) - dx * sin(xyth);

      gCameraFrom.x = gCameraTo.x + xx*10.0;
      gCameraFrom.y = gCameraTo.y + yy*10.0;


    }
  else
#endif
  if(keyflags & MK_RBUTTON)
    {

      double mvz = (y - oy) / 10.0;

      gCameraTo.x = gCameraFrom.x + xx*10.0;
      gCameraTo.y = gCameraFrom.y + yy*10.0;
      gCameraTo.z = gCameraFrom.z + dz*10.0 - mvz;

    }

  if((keyflags & (MK_MBUTTON | MK_SHIFT | MK_LBUTTON | MK_RBUTTON)) &&
     (x != ox || y != oy))
    {
      if(m_camera)
      {
	m_camera->SetViewFrom(DBL2D3DVECTOR(gCameraFrom));
	m_camera->SetViewAt(DBL2D3DVECTOR(gCameraTo));
	m_camera->FInit();
      }
    }

  ox = x;
  oy = y;

}

void CDXFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
  OnMouseMove(m_hWnd, point.x, point.y, nFlags);
  if(m_bWindowed)
    CFrameWnd::OnMouseMove(nFlags, point);
}

void CDXFrame::SetFullScreenMode(bool fl, bool doflag)
{
  if(!doflag)
    { // �t���O�̐ݒ�̂�
      m_ForceWindowMode = m_bWindowed = !fl;
      return;
    }

  if(fl == m_bWindowed)
    { // �`�F���W���[�h
      if (m_bActive && m_bReady) 
	{
	  m_bReady = FALSE;
	  m_bWindowed = !m_bWindowed;
	  if(m_framework) ChangeDirect3D();
	  m_bReady = TRUE;
	  m_ForceWindowMode = m_bWindowed;
	}
      else
	{ // �ł��Ȃ������ł��
	  m_ForceWindowMode = !m_bWindowed;
	}
    }
}


void	CDXFrame::OnKey(HWND hwnd, UINT vk, BOOL down, int repeat, UINT flags)
{
  if (down) {
    switch( vk ) {
    case VK_F10:
      {
	return;
      }
      break;
#if 0		  
    case VK_RETURN: // ALT+
      
      if(GetAsyncKeyState(VK_MENU) < 0)
	{
	  SetFullScreenMode(m_bWindowed);
	}


      if (m_bActive && m_bReady) {
	m_bReady = FALSE;
	m_bWindowed = !m_bWindowed;
	ChangeDirect3D();
	m_bReady = TRUE;
      }
#endif
      break;
      
    case VK_ESCAPE:
      {
	if(!m_p3DObjects->OnESC())
	  {
	    MusicStop();
	    // Window���[�h�ɖ߂�
	    SetFullScreenMode(false);
	    
	    if(GetAsyncKeyState(VK_TAB) < 0)
	      {
		PostMessage(WM_CLOSE);
		return;
	      }
	    m_bInitMode = true;
	    if(!m_p3DObjects->Initialize())
	      { // �������Ɏ��s������I��
		PostMessage(WM_CLOSE);
	      }
	  }
      }
      break;
    default:
      if(m_bWindowed)
	CWnd::OnKeyDown(vk, repeat, flags);
    }
  }
}

void CDXFrame::OnMenuInit()
{
  // Window���[�h�ɖ߂�
  SetFullScreenMode(false);

  m_bInitMode = true;
  if(!m_p3DObjects->Initialize())
    { // �������Ɏ��s������I��
      PostMessage(WM_CLOSE);
    }
}


void CDXFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if(m_bImeOpen)
    {
      ::ImmSimulateHotKey(m_hWnd, IME_JHOTKEY_CLOSE_OPEN);
      ::ImmSetOpenStatus(m_hImc, FALSE);
      m_bImeOpen = false;
    }
  OnKey(m_hWnd, nChar, TRUE, nRepCnt, nFlags);

}


void CDXFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if(m_bWindowed)
    CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}





void	CDXFrame::OnPaint(HWND hwnd)
{
  if( m_framework ) {
    if (!m_bReady && !m_bWindowed)
      _XDraw->FlipToGDISurface( TRUE );
  }

  FORWARD_WM_PAINT( hwnd, ::DefWindowProc);
}

void CDXFrame::OnPaint() 
{
  //CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g
  OnPaint(m_hWnd);
}


void CDXFrame::OnMove(HWND hwnd, int x, int y)
{
  if( m_bActive && m_bReady && m_bWindowed ) 
    {
      m_framework->Move( x, y );
    }
  if(m_bWindowed)
    {
      FORWARD_WM_MOVE( hwnd, x, y, ::DefWindowProc);
    }
}
void CDXFrame::OnMove(int x, int y) 
{
  OnMove(m_hWnd, x, y);
//  CFrameWnd::OnMove(x, y);

  m_p3DObjects->OnMove(x,y);
}




void	CDXFrame::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
  if (SIZE_MAXHIDE==state || SIZE_MINIMIZED==state)
    m_bActive = FALSE;
  else 
    m_bActive = TRUE;

  if (m_bActive && m_bReady && m_bWindowed) {
    m_bReady = FALSE;
    if(m_framework) ChangeDirect3D();
    m_bReady = TRUE;
	}
  
  FORWARD_WM_SIZE( hwnd, state, cx, cy, ::DefWindowProc);
}

void CDXFrame::OnSize(UINT nType, int cx, int cy) 
{
  OnSize(m_hWnd, nType, cx, cy);
//	CFrameWnd::OnSize(nType, cx, cy);
}



void	CDXFrame::OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO mmi)
{
	mmi->ptMinTrackSize.x = 100;
	mmi->ptMinTrackSize.y = 100;

	FORWARD_WM_GETMINMAXINFO( hwnd, mmi, ::DefWindowProc);
}
void CDXFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
  OnGetMinMaxInfo(m_hWnd, lpMMI);
  //CFrameWnd::OnGetMinMaxInfo(lpMMI);
}




BOOL	CDXFrame::OnSetCursor(HWND hwnd, HWND hwndCursor, UINT hitCode, UINT msg)
{
  if (m_bActive && m_bReady && (!m_bWindowed) ) {
    SetCursor(NULL);
    return TRUE;
  }
  if(m_bWindowed)
    FORWARD_WM_SETCURSOR( hwnd, hwndCursor, hitCode, msg, ::DefWindowProc);
  return FALSE;
}

BOOL CDXFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  if(m_bWindowed)
    return CFrameWnd::OnSetCursor(pWnd, nHitTest, message);
  else
    return TRUE;
}







BOOL	CDXFrame::OnContextMenu(HWND hwnd, HWND hwndctl, int x, int y)
{
	return TRUE;
}


void	CDXFrame::OnClose(HWND hwnd)
{
	DestroyWindow();
}

void CDXFrame::OnClose() 
{
  CFrameWnd::OnClose();
}


void	CDXFrame::OnActivate(HWND hwnd, UINT state, HWND hActive, BOOL minimized)
{
  if ((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)) {
    if (m_bActive && m_bReady && (!m_bWindowed)) {
      m_bReady = FALSE;
      if(m_framework) ChangeDirect3D();
      m_bReady = TRUE;
    }
  }
}

void CDXFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
  if(pWndOther)
  OnActivate(m_hWnd, nState, pWndOther->m_hWnd, bMinimized);
  // CWnd::OnActivate(nState, pWndOther, bMinimized);
}



void	CDXFrame::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
  switch( id ) {
  case ID_INITIALUPDATE:

    ::ImmAssociateContext(m_hWnd, m_hImc);

    FInit(SW_SHOW);
    SetReady(TRUE);
  }
//	case IDM_EXIT:
//		SendMessage( hwnd, WM_CLOSE, 0, 0);
//		break;
//	}
}

BOOL CDXFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  switch(wParam)
    {
    case ID_INITIALUPDATE:
      FInit(SW_SHOW);
      SetReady(TRUE);
      break;
    }
  return CFrameWnd::OnCommand(wParam, lParam);
}









BOOL CDXFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
  if (!CFrameWnd::PreCreateWindow(cs))
    return FALSE;

  cs.dwExStyle |= WS_EX_CLIENTEDGE;
  cs.style &= ~WS_BORDER;
  cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
				     ::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);


  HDC dc = CreateCompatibleDC(NULL);

  CRect rc;
  ::GetWindowRect(NULL, &rc);
  int w = GetDeviceCaps(dc, HORZRES);
  int h = GetDeviceCaps(dc, VERTRES);

  DeleteDC(dc);

  cs.x = (w - gResolution.cx) / 2;
  cs.y = (h - gResolution.cy) / 2;

  cs.cx = gResolution.cx; //1024; // Window�̏����l��1024x600dot
  cs.cy = gResolution.cy; //600;

  if(cs.x < 0) cs.x = 0;
  if(cs.y < 0) cs.y = 0;
  if(cs.cx > w) cs.cx = w;
  if(cs.cy > h) cs.cy = h;



//  ::ImmDisableIME(0); //this->m_nThreadID);


#if 0
	WNDCLASS	wndclass;
	//�E�B���h�E�N���X�o�^
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= FullScreenWndProc;
	wndclass.cbClsExtra 	= 0;
	wndclass.cbWndExtra 	= sizeof( long );
	wndclass.hInstance	= m_hInstance;
	wndclass.hIcon		= LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor	= NULL; //LoadCursor( NULL, IDC_ARROW );
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= FRAMECLASS;

	::RegisterClass( &wndclass );
#endif

	return TRUE;
}

void CDXFrame::OnFileLoad() 
{
  m_p3DObjects->LoadNewData();
}

void CDXFrame::SetCamera()
{
  m_camera->SetViewFrom(DBL2D3DVECTOR(gCameraFrom));
  m_camera->SetViewAt(DBL2D3DVECTOR(gCameraTo));
  m_camera->FInit();
  m_camera->Active();
}

void CDXFrame::OnLoadmap() 
{
  m_p3DObjects->LoadMapData();
}

void CDXFrame::OnSavemap() 
{
  m_p3DObjects->SaveMapData();
}


void CDXFrame::OnEditMapitem() 
{
  m_p3DObjects->EditMapItem();
}

void CDXFrame::OnMenuResult() 
{
#ifdef SLBENCH

  MusicStop();
  // Window���[�h�ɖ߂�
  SetFullScreenMode(false);
  CResultDialog& dlg = *m_pResultDialog;

  const char* onoff[2] = {"OFF", "ON" };
  double btm = (m_BenchEndTime - m_BenchStartTime) / 1000.0;

  CString st;
  dlg.m_mes = "SL BENCH Ver " VERSION "\r\n";
  st.Format("------------------------------------------------------------\r\n");
  dlg.m_mes += st;
  st.Format("%d x %d dot %dbit colormode\r\n", m_BenchWidth, m_BenchHeight, m_BenchDeps);
  dlg.m_mes += st;

  st.Format("BilinearFilter:%s, AlphaBrending:%s, Specular:%s\r\n",
	    onoff[gBiLinearEnable], onoff[gAlphaBrendingEnable], onoff[gSpecularEnable]);
  dlg.m_mes += st;
  st.Format("Dither:%s, AntiAlias:%s, TableFog:%s, VertexFog:%s\r\n",
	    onoff[gDitherEnable], onoff[gAntiAliasEnable], onoff[gTableFogEnable],
	    onoff[gVertexFogEnable]);
  dlg.m_mes += st;
  st.Format("FarPlane:%6.1fm, SmokePolygons:%d\r\n",   gSikai, gSmokeVMAX);
  dlg.m_mes += st;
  st.Format("------------------------------------------------------------\r\n");
  dlg.m_mes += st;



  st.Format("Benchmark Time         %9.2lf sec\r\n", btm);
  dlg.m_mes += st;
  st.Format("Total Frames         %8d frames\r\n", m_BenchFrames);
  dlg.m_mes += st;
  st.Format("Average FPS            %9.2lf fps\r\n", m_BenchFrames / btm);
  dlg.m_mes += st;
  st.Format("Fastest FPS            %9.2lf fps\r\n", 1000.0 / m_MinTime);
  dlg.m_mes += st;
  if(m_MaxTime < m_MinTime)
    st.Format("Lowest FPS             %9.2lf fps\r\n", 0.0);
  else
    st.Format("Lowest FPS             %9.2lf fps\r\n", 1000.0 / m_MaxTime);
  dlg.m_mes += st;
  st.Format("Polygon Performance    %9.2lf Kpolygons/sec\r\n", m_BenchPolygons / 1000.0 / btm);
  dlg.m_mes += st;
  st.Format("Fastest PPS            %9.2lf Kpolygons/sec\r\n", m_BestPolygons / 1000.0);
  dlg.m_mes += st;
  st.Format("Vertex Performance     %9.2lf Kvertexes/sec\r\n", m_BenchVertexes  / 1000.0/ btm);
  dlg.m_mes += st;
  st.Format("Fastest VPS            %9.2lf Kvertexes/sec\r\n", m_BestVertexes / 1000.0);
  dlg.m_mes += st;


  dlg.DoModal();

#else // else SLBENCH

  // Window���[�h�ɖ߂�
  Sleep(1000);
  SetFullScreenMode(false);
  m_p3DObjects->ShowResultDialog();

#endif


  m_bInitMode = true;
  if(!m_p3DObjects->Initialize())
    { // �������Ɏ��s������I��
      PostMessage(WM_CLOSE);
    }


	
}

void CDXFrame::StartBench(DWORD curtime)
{
  m_BenchStartTime = curtime;
  m_BenchFrames = 0;
  m_MinTime = 10 * 1000;
  m_MaxTime = 1;
  m_BenchPolygons = 0;
  m_BenchVertexes = 0 ;
  m_BenchEndTime = 0;
  m_BestPolygons = 0.0;
  m_BestVertexes = 0.0;

}
void CDXFrame::StopBench(DWORD curtime, bool fdlg)
{
  m_BenchEndTime = curtime;
  if(fdlg)
    PostMessage(WM_COMMAND, ID_MENU_RESULT);
}



/////////////////////////////////////////////////////////////////////////////
// CColorBox ���b�Z�[�W �n���h��

CBrush nullbr(NULL_BRUSH);

HBRUSH CDXFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
  return(HBRUSH)nullbr;
}

HBRUSH CDXFrame::CtlColor(CDC* pDC, UINT nCtlColor) 
{
  return(HBRUSH)nullbr;
}

BOOL CDXFrame::OnEraseBkgnd(CDC* pDC) 
{
  CRect rc;
  GetClientRect(&rc);
  pDC->FillSolidRect(rc, RGB(0,0,0));
  m_p3DObjects->OnEraseBkgnd(pDC);
  return TRUE;	
}



void CDXFrame::MusicStop()
{
//  PlaySound(NULL, NULL, SND_PURGE);
}






// �J�����̈ړ�
void CDXFrame::MoveCamera()
{
  CPoint pos;
  GetCursorPos(&pos);
  ScreenToClient(&pos);
  CRect rc;
  GetClientRect(&rc);
  int cx = rc.Width() / 2;
  int cy = rc.Height() / 2;
  pos.x -= cx;
  pos.y -= cy;

  double mvzoom = (-pos.y / 10.0) *  (gPastTime / 1000.0);
  double mvturn = (-pos.x / 25.0) * (gPastTime / 1000.0);
    

  double dx = gCameraFrom.x - gCameraTo.x;
  double dy = gCameraFrom.y - gCameraTo.y;
  double dz = gCameraFrom.z - gCameraTo.z;
  double k = sqrt(1.0/(dx*dx+dy*dy+dz*dz));
  dx *= -k;
  dy *= -k;
  dz *= -k;
  
  if(m_LButton)
    { // L�{�^����������Ă�����


      gCameraTo.x = gCameraFrom.x + dx*10.0;
      gCameraTo.y = gCameraFrom.y + dy*10.0;
      gCameraTo.z = gCameraFrom.z + dz*10.0;


      dx *= mvzoom;
      dy *= mvzoom;
      dz *= mvzoom;
      gCameraFrom.x += dx;
      gCameraFrom.y += dy;
      gCameraFrom.z += dz;
      gCameraTo.x += dx;
      gCameraTo.y += dy;
      gCameraTo.z += dz;

      // �}�E�X�̍��E�Ŏ��_�����ɃJ��������]������
      dx = gCameraTo.x - gCameraFrom.x;
      dy = gCameraTo.y - gCameraFrom.y;
      dz = gCameraTo.z - gCameraFrom.z;
      double k = sqrt(1.0/(dx*dx+dy*dy+dz*dz));
      dx *= -k;
      dy *= -k;
      double xyth = mvturn * PI / 180.0;
      double xx = dx * cos(xyth) + dy * sin(xyth);
      double yy = dy * cos(xyth) - dx * sin(xyth);

      gCameraFrom.x = gCameraTo.x + xx*10.0;
      gCameraFrom.y = gCameraTo.y + yy*10.0;
      
    }
  
  if(m_camera)
    {
      m_camera->SetViewFrom(DBL2D3DVECTOR(gCameraFrom));
      m_camera->SetViewAt(DBL2D3DVECTOR(gCameraTo));
      m_camera->FInit();
    }

}



void CDXFrame::OnLButtonUp(UINT nFlags, CPoint point) 
{
  m_LButton = false;
  if(m_bWindowed)
    CFrameWnd::OnLButtonUp(nFlags, point);
  m_p3DObjects->OnLButtonUp(nFlags, point);
}


void CDXFrame::OnTogglefullscreen() 
{
  SetFullScreenMode(m_bWindowed);
}

void CDXFrame::OnEndInitDialog()
{
  if(!m_p3DObjects->Initialize2())
    { // �������Ɏ��s������I��
      PostMessage(WM_CLOSE);
    }
  m_bInitMode = false;
}

BOOL CDXFrame::PreTranslateMessage(MSG* pMsg) 
{
if(pMsg->message == WM_KEYDOWN)
{
///  TRACE("%x %x\n", pMsg->wParam, VK_F10);
}
	return CFrameWnd::PreTranslateMessage(pMsg);
}



void CDXFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
  char *p;
  switch(nID & 0xfff0)
    {
    case SC_CLOSE: // 
      p = "  CWnd �I�u�W�F�N�g���N���[�Y���܂��B"; break;
    case SC_HOTKEY: // 
      p = "  �A�v���P�[�V�����w��̃z�b�g �L�[�Ɋ֘A���� CWnd �I�u�W�F�N�g���A�N�e�B�u�ɂ��܂��BlParam �̉��ʃ��[�h�ŃA�N�e�B�u�ɂȂ�E�B���h�E�� HWND �����ʂ��܂��B"; break;
    case SC_HSCROLL: // 
      p = "  �����ɃX�N���[�����܂��B"; break;
    case SC_KEYMENU: // 
      p = "  �L�[����ɂ�胁�j���[���擾���܂��B"; break;
    case SC_MAXIMIZE: // 
      p = "(�܂��� SC_ZOOM)   CWnd �I�u�W�F�N�g���ő�\�����܂��B"; break;
    case SC_MINIMIZE: // 
      p = "(�܂��� SC_ICON)   CWnd �I�u�W�F�N�g���A�C�R�������܂��B"; break;
    case SC_MOUSEMENU: // 
      p = "  �}�E�X �N���b�N�ɂ�胁�j���[���擾���܂��B"; break;
    case SC_MOVE: // 
      p = "  CWnd �I�u�W�F�N�g���ړ����܂��B"; break;
    case SC_NEXTWINDOW: // 
      p = "  ���̃E�B���h�E�Ɉړ����܂��B"; break;
    case SC_PREVWINDOW: // 
      p = "  ���O�̃E�B���h�E�Ɉړ����܂��B"; break;
    case SC_RESTORE: // 
      p = "  ���̈ʒu�ƃT�C�Y�ɃE�B���h�E��߂��܂��B"; break;
    case SC_SCREENSAVE: // 
      p = "  SYSTEM.INI �t�@�C���� [boot] �Z�N�V�����Ŏw�肳�ꂽ�X�N���[�� �Z�[�o�[ �A�v���P�[�V���������s���܂��B"; break;
    case SC_SIZE: // 
      p = "  CWnd �I�u�W�F�N�g�̃T�C�Y��ύX���܂��B"; break;
    case SC_TASKLIST: // 
      p = "  Windows �̃^�X�N �}�l�[�W�������s�܂��̓A�N�e�B�u�ɂ��܂��B"; break;
    case SC_VSCROLL: // 
      p = "  �����ɃX�N���[�����܂��B "; break;
    default:
      p = "?";
    }
  TRACE("nID=%d LPARAM=%d mes=%s\n", nID, lParam, p);

  if(nID == SC_KEYMENU) return;
  CFrameWnd::OnSysCommand(nID, lParam);
}






LRESULT CDXFrame::OnMyImeNotify(WPARAM dwCommand, LPARAM dwData)
{
  if(dwCommand == 8)
    {
      if(!m_bWindowed) SetFullScreenMode(m_bWindowed);
      m_bImeOpen = true;
    }
  // CG: ���̊֐��� �uIME �T�|�[�g�v �R���|�[�l���g�ɂ��ǉ�����Ă��܂��B
  return DefWindowProc(WM_IME_NOTIFY, dwCommand, dwData);
}


#if 0
void CDXFrame::CgAdjustCompositionWindow()
{
  // CG: ���̊֐��� �uIME �T�|�[�g�v �R���|�[�l���g�ɂ��ǉ�����Ă��܂��B
  // ���݂̃L�����b�g�ʒu�ɕϊ��E�B���h�E���ړ�����K�v������ꍇ�ɁA���̊֐����Ăяo���Ă��������B
  COMPOSITIONFORM cf;
  cf.dwStyle = CFS_POINT;
  bool flag = false;
  CPoint p(pItem->MyRect.left+2, pItem->MyRect.top+1);
  CPoint scr = GetDeviceScrollPosition();
  p.y -= scr.y;

  cf.ptCurrentPos = p;
  if(pItem->Attr(HS_Kanji))  flag = true;
  ::ImmSetCompositionWindow(m_hImc, &cf);

  ::ImmSetOpenStatus(m_hImc, flag);

  {
    HWND hw = ::ImmGetDefaultIMEWnd(m_hWnd);
    HIMC himc = ::ImmGetContext(hw);
    COMPOSITIONFORM cf;
    cf.dwStyle = CFS_POINT;
    cf.ptCurrentPos = CPoint(0,0);
    ::ImmSetCompositionWindow(himc, &cf);
    ::ImmSetOpenStatus(himc, flag);
  }

}

#endif
