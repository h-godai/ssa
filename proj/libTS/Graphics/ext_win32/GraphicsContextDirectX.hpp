/* GraphicsContextDirectX.h


*/


#ifndef TS_EXTWIN32_GraphicsCOntextDirectX_H
#define TS_EXTWIN32_GraphicsCOntextDirectX_H

#include "Graphics/IGraphicsContext.h"

#ifndef TS_PLATFORM_WIN32
#error
#else
#ifndef _AFXDLL
#include <windows.h>
#endif
#endif

namespace ts {

  class GraphicsContextDirectX : public GraphicsContext {

  public:
    GraphicsContextDirectX()
      : m_handle(0)
      , m_hglrc(0)
      , m_hdc(0)
      , m_clearColor(0,0,0,0)
      , m_lock(0)
    {}
    GraphicsContextDirectX(HWND handle)
      : m_handle(handle)
      , m_hglrc(0)
      , m_hdc(0)
      , m_clearColor(0,0,0,0)
      , m_lock(0)
    {}
    virtual ~GraphicsContextDirectX() {
      if (m_hglrc != 0) {
        //DirectXDeleteContext(m_hglrc);
      }
    }


    virtual void beginFrame() {
#if 0
      if (m_hdc == 0) return;
      // make it the calling thread's current rendering context
      if (::DirectXMakeCurrent (m_hdc, m_hglrc) == FALSE) {
        // error
        return;
      }

      
      ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      ::glAlphaFunc(GL_GREATER, 0.1f);
      ::glEnable(GL_ALPHA_TEST);

      // call OpenGL APIs as desired ...
      ::glClearColor(m_clearColor.getRf(), m_clearColor.getGf(), m_clearColor.getBf(), 1.0f);
      ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      ::glEnable(GL_DEPTH_TEST);
      ::glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	  ::glEnable(GL_CULL_FACE);
	  //::glDisable(GL_CULL_FACE);
      ::glLoadIdentity();
#endif
    }

    virtual void endFrame() {
#if 0
      if (m_hdc == 0) return;

      if (++m_lock == 1) {
        for (vector<uint32_t>::iterator i = m_deleteTextures.begin(); i != m_deleteTextures.end(); ++i) {
          GLuint handle = *i;
          ::glDeleteTextures(1, &handle);
        }
        m_deleteTextures.clear();
        --m_lock;
      }

      SwapBuffers(m_hdc);
      // when the rendering context is no longer needed ...  
      // make the rendering context not current 
      DirectXMakeCurrent (NULL, NULL) ; 
#endif
    }
    virtual void setClearColor(const ColorRGBA& col) {
      m_clearColor = col;
    }

    virtual void setViewPort(const ts::RectS32& rect) {
      m_rect = rect;
//      ::glViewport(m_rect.left, m_rect.top, m_rect.width, m_rect.height);
    }

    void setHDC(HDC hdc) {
# if 0
      m_hdc = hdc;
      if (m_hglrc == 0) {
	        initDirectX(m_hdc);
      }
#endif
    }

    void setContextId(IGraphicsContext::ContextId id) { m_contextId = id; }

    virtual void deleteTexture(uint32_t handle) {
      while (m_lock > 0) Sleep(0);
      ++m_lock;
      m_deleteTextures.push_back(handle);
      --m_lock;
    }


  protected:
    void initDirectX(HDC hdc) {

    PIXELFORMATDESCRIPTOR pfd = { 
	sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
	1,                     // version number 
	PFD_DRAW_TO_WINDOW |   // support window 
	PFD_SUPPORT_OPENGL |   // support OpenGL 
	PFD_DOUBLEBUFFER,      // double buffered 
	PFD_TYPE_RGBA,         // RGBA type 
	24,                    // 24-bit color depth 
	0, 0, 0, 0, 0, 0,      // color bits ignored 
	0,                     // no alpha buffer 
	0,                     // shift bit ignored 
	0,                     // no accumulation buffer 
	0, 0, 0, 0,            // accum bits ignored 
	32,                    // 32-bit z-buffer 
	0,                     // no stencil buffer 
	0,                     // no auxiliary buffer 
	PFD_MAIN_PLANE,        // main layer 
	0,                     // reserved 
	0, 0, 0                // layer masks ignored 
    }; 
    int iPixelFormat = ChoosePixelFormat(hdc, &pfd); 
    SetPixelFormat(hdc, iPixelFormat, &pfd); 

	// create a rendering context 
   // m_hglrc = DirectXCreateContext (hdc); 
  }

  private:
    RectS32 m_rect;
    HWND m_handle;
    HGLRC m_hglrc;
    HDC m_hdc;
    ColorRGBA m_clearColor;
    vector<uint32_t> m_deleteTextures;
    volatile int m_lock; // 簡易mutex

  };


  namespace dx {
    typedef void *CTextureBuffer;
    //カラービット配置情報
typedef	struct tagRGB16 {
	RGBQUAD	depth;		//ビット深度
	RGBQUAD	amount;		//shift
	RGBQUAD	position;	//ワードポジション
} RGB16;

#define	LPRGB16		RGB16 *


//16bit bitmap 転送情報
typedef struct tagBLTINFO16 {
	LPWORD	lpbitmap;		//ビットマップ本体
	int		linesize;		//水平バイトサイズ
	int		dx,dy;			//ビットマップの大きさ
} BLTINFO16;

#define	LPBLTINFO16 BLTINFO16 *

//8bit bitmap 転送情報
typedef struct tagBLTINFO8 {
	RGBQUAD	*lprgb;			//パレットデータ(256color with APLHA)
	LPWORD	lpbitmap;		//ビットマップ本体
	int		linesize;		//水平バイトサイズ
	int		dx,dy;			//ビットマップの大きさ
} BLTINFO8;

#define	LPBLTINFO8 BLTINFO8 *

#if 0
    class	CSurface
{
protected:
	LPDIRECTDRAWSURFACE7	m_lpFace;
	DDSURFACEDESC2			m_ddsd;

	int			m_clipx0;
	int			m_clipy0;
	int			m_clipx1;
	int			m_clipy1;

	RGB16 		m_rgb16;

	BOOL	GetRGB16Bit();
public:
	CSurface();
	~CSurface();

	HRESULT		FInit(int width, int height);
	LPDIRECTDRAWSURFACE7	FInitPrimary(DDSURFACEDESC2 *ddsd);
	LPDIRECTDRAWSURFACE7	FInitBack(CSurface * primary, DDSCAPS2 *ddscaps);
	HRESULT		FInitTexture(int dx, int dy);

	LPDIRECTDRAWSURFACE7	SURFACE();

	HRESULT		Lock();
	HRESULT		Unlock();

	HRESULT		Restore();
	void		Flip();
	HRESULT		SetClipper(LPDIRECTDRAWCLIPPER clip);
	HRESULT		AddAttachedSurface(LPDIRECTDRAWSURFACE7 srf);

	RGB16 *		GetRGB16();
	WORD		GetRGB16Color(BYTE red, BYTE green, BYTE blue);
	void		Get16Bit(BLTINFO16 *pic, int dx, int dy);

	void	FastBlt(BLTINFO16 *pic, int x0, int y0, int dx, int dy, int x2, int y2);
	void	FastBltFX(BLTINFO16 *pic, int x0, int y0, int dx, int dy, int x2, int y2, LPBYTE mask);
	void	AlphaBlt(BLTINFO16 *pic, BLTINFO16 *pic2, int x0, int y0, int dx, int dy, int x2, int y2, WORD level);
	void	KeyBlt(BLTINFO16 *pic, int x0, int y0, int dx, int dy, int x2, int y2);
};
class	CXPalette
{
protected:
	LPDIRECTDRAWPALETTE		m_pal;
	PALETTEENTRY			m_pe[256];
	BOOL					m_fAlpha;

public:
	CXPalette(BOOL fAlpha);
	~CXPalette();
	BOOL	FInit(RGBQUAD * rgb);
	BOOL	FInit(LPBYTE rgb);
	BOOL	FInit(LPWORD rgb);
	BOOL	Copy(CXPalette * src);

	LPDIRECTDRAWPALETTE	PALETTE();
};

#define	LPCXPalette		CXPalette *
typedef	CSurface *		LPCSurface;
class	CTexture
{
protected:
  LPDIRECTDRAWSURFACE7	m_surface;
  HBITMAP		m_hbitmap;

  int			m_dx;
  int			m_dy;
  DWORD			m_dwBPP;

  HRESULT		CopyFromBitmap();
  HRESULT		RestoreFromBitmap();

  CTextureBuffer* m_TexBuf; // h.godai
public:
  tTextureID m_ID;
  int m_ReferenceCnt;
  int m_ReferenceCnt2;
  CString m_FileName;
  DWORD m_LastActiveTime;
  
  int m_Status; 
  enum{ SLEEP, 	// 待機状態 (データは読み込まれない)
	  ACTIVE,	// いつでも描画できる
	  WAKEUP,     // データロード中
	  LOADED,	// データロード終了
	  NOFILE	// エラー
	};

  
  CTexture();
  ~CTexture();

  void SetSleep();
  void SetActive(bool fWait = true);



  int GetSize();

  HRESULT FInit(int dx, int dy, int bit, char* fname);

  HRESULT Restore();
  void	  Release();
  HRESULT Resume();


  HRESULT SetPicture(LPBLTINFO8 bltinfo);
  HRESULT SetBitmapFile(const char* filename);
  HRESULT SetBitmapFile2(LPBYTE btmp);
  HRESULT Upload(LPCSurface sysmemsrf, LPCXPalette pal);
  HRESULT SetColorKey(DWORD dwColor);

  HRESULT Active(int=0);


  // ここからはh.godaiが追加

  HRESULT SetTextureBuffer(CTextureBuffer* tbf);

};

typedef	CTexture* LPCTexture;

    class	CX3DCamera
{
protected:
	CD3DMatrix	m_viewmatrix;
	CD3DMatrix	m_projectmatrix;

	D3DVECTOR	m_from;
	D3DVECTOR	m_at;
	D3DVECTOR	m_up;
	float	m_fov;
	float	m_near_plane;
	float	m_far_plane;

public:
	CX3DCamera();
	~CX3DCamera();

	BOOL	FInit();
	void	SetProjection(float near_plane, float far_plane, float fov);
	void	SetViewFrom(D3DVECTOR &from);
	void	SetViewAt(D3DVECTOR &at);
	void	SetViewUp(D3DVECTOR &up);
	void	CalcViewMatrix();
	void	CalcProjectionMatrix();
	void	CalcProjectionMatrixVP(int dx, int dy);
	HRESULT	Active();

};

#define	LPCX3DCamera	CX3DCamera *

class	CX3DViewport
{
protected:
	D3DVIEWPORT7			m_view;

	int			m_x0,m_y0;
	int			m_dx,m_dy;

public:
	CX3DViewport();
	~CX3DViewport();

	HRESULT		FInit(CX3DCamera * camera, int x0, int y0, int dx, int dy);
	HRESULT		FromWindow(CX3DCamera * camera, HWND hwnd);
	HRESULT		Active(CX3DCamera * camera);
};

#define	LPCX3DViewport	CX3DViewport *

class	CX3DDraw
{
private:
	LPDIRECT3D7			m_lpD3D;
	LPDIRECT3DDEVICE7	m_lpDevice;
	BOOL				m_bHAL;
	DDPIXELFORMAT		m_ddpfZBuffer;
	LPDIRECTDRAWSURFACE7	m_zbuffer;
	CX3DViewport *		m_viewport;


public:
	CX3DDraw();
	~CX3DDraw();

	HRESULT		FInit(GUID* pDeviceGUID);
	HRESULT		CreateZBuffer(GUID* pDeviceGUID);

	void		GetDeviceCaps(D3DDEVICEDESC7 *ddDesc);
	HRESULT		RestoreSurfaces();
	HRESULT		ReleaseZBuffer();
	LPDIRECT3D7 	Direct3D();
	LPDIRECT3DDEVICE7	DEVICE();
	BOOL		IsHAL();
	LPDIRECTDRAWSURFACE7	ZBUFFER();
	LPCX3DViewport	VIEWPORT();
	HRESULT 		SetRenderTarget(LPDIRECTDRAWSURFACE7 pddsRenderTarget);

	HRESULT BeginScene();
	HRESULT EndScene();
	HRESULT ActiveFullscreenViewport();
	HRESULT SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD param);
	HRESULT SetWorldTransform(CD3DMatrix * mat);
	HRESULT SetViewTransform(CD3DMatrix * mat);
	HRESULT SetProjectionTransform(CD3DMatrix * mat);
	HRESULT SetZBufferWriteEneble(BOOL fEnable);
	HRESULT SetAntialias(BOOL fEnable);
	HRESULT SetColorkeyEnable(BOOL fEnable);
	HRESULT SetTransparent(BOOL fEnable);
	HRESULT SetTransparentADD();
	HRESULT SetTransparentSUB();
	void SetAlphaHardwareEnable(BOOL fEnable);

	HRESULT	DrawPrimitive(D3DPRIMITIVETYPE type, DWORD vtd, LPVOID lpvertex, DWORD num);
	HRESULT	DrawIndexedPrimitive(D3DPRIMITIVETYPE type, DWORD vtd, LPVOID lpvertex, DWORD vnum, LPWORD lpindex, DWORD inum);
};

typedef CX3DDraw *		LPCX3DDraw;










class	CXDraw
{
private:
public:
	LPDIRECTDRAW7			m_lpDD;
	LPDIRECTDRAWCLIPPER		m_clip;

	CSurface *		m_FrontSurface;		//プライマリ
	CSurface *		m_BackSurface;		//バック

	int		m_bit;
	int		m_dx,m_dy;
	HWND	m_hwnd;

	RECT	m_rcViewportRect;
	RECT	m_rcScreenRect;
	LPDIRECTDRAWSURFACE7	m_pddsFrontBuffer;
	LPDIRECTDRAWSURFACE7	m_pddsBackBuffer;
	BOOL	m_fFullscreen;

public:
	CXDraw();
	~CXDraw();

	HRESULT	FInit(HWND hwnd, int dx, int dy, GUID* pDriverGUID, BOOL fFullScreen);
	HRESULT	CreateBuffer(DDSURFACEDESC2* pddsd);

  CSurface *	PRIMARY();
	CSurface *	BACK();
	int		DX();
	int		DY();
	int		BIT();
	RECT*   GetViewportRect()		{ return &m_rcViewportRect; }
	BOOL	IsFullscreen();
	
	HRESULT		Flip();
	HRESULT		FlipToGDISurface( BOOL bDrawFrame );
	HRESULT		RestoreSurfaces();
	void		Move(int x, int y);

};

typedef	CXDraw *		LPCXDraw;
  class	CX3DFrame
  {
  private:
	  LPCXDraw	m_xdraw;	
	  LPCX3DDraw	m_x3ddraw;

	  HWND		m_hwnd;
	  BOOL		m_fFullscreen;

	  HRESULT		create_default_scene();
  public:
	  CX3DFrame();
	  ~CX3DFrame();

	  HRESULT		FInit(HWND hframewindow, 
						   GUID * pDriverGUID,
						   GUID * pDeviceGUID,
						   DDSURFACEDESC2 * pMode,
						   BOOL );
	  HRESULT		RestoreSurfaces();
	  HRESULT		Move(int x, int y);
  };

  }

#endif
} // namespace

#endif
