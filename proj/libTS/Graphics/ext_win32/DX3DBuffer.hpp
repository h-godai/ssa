/* DX3DBuffer.hpp */


#ifndef TS_GRAPHICS_EXTWIN32_DX3DBUFFER_HPP
#define TS_GRAPHICS_EXTWIN32_DX3DBUFFER_HPP

#pragma warning (disable : 4201)

#include <d3d.h>
//#include <d3dx.h>
#include <ddraw.h>
//#include <dinput.h>
#pragma warning (disable : 4201)

#include <mmsystem.h>
#include <windowsx.h>


//ライブラリの直接指定
#pragma comment( lib,"dxguid.lib" )
#pragma comment( lib,"ddraw.lib" )
#pragma comment( lib,"dinput.lib" )
#pragma comment( lib,"winmm.lib" )


//------------------------------------------------------------------

//安全対策版DELETE & RELEASE
#ifndef	SAFE_DELETE
#define SAFE_DELETE(p)	{ if(p) { delete (p);		(p)=NULL; } }
#endif
#ifndef	SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release();	(p)=NULL; } }
#endif
#ifndef	SAFE_DELETES
#define SAFE_DELETES(p) { if(p) { delete [] (p);	(p)=NULL; } }
#endif



namespace ts {
  namespace dx {

  class DX3DVertexBuffer {

	LPDIRECT3DVERTEXBUFFER7	m_vbuffer;
	DWORD				m_dwFVF;
	DWORD				m_dwNumVertices;

	DWORD				m_dwStride;
  public:
	DX3DVertexBuffer();
	~DX3DVertexBuffer();

	HRESULT	create(DWORD dwFVF, DWORD dwNumVertices);

	HRESULT	getCaps(D3DVERTEXBUFFERDESC * vbdesc);
	HRESULT	setVertex(LPVOID vertex, DWORD num);
	
	HRESULT	lock( DWORD dwFlags, LPVOID * lpVertex, LPDWORD lpdwSize);
	HRESULT	unlock();
	HRESULT	drawPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType);
	HRESULT	drawIndexedPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType, 
											   LPWORD lpwIndices, 
											   DWORD dwIndexCount);
	
	HRESULT	optimize();
};



  } // namespace
} // namespace










#endif

