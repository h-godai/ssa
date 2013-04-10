#include "stdafx.h"
#pragma warning (disable : 4201)
#include "DX3DBuffer.hpp"


extern LPDIRECT3D7			_Direct3D;
extern LPDIRECT3DDEVICE7	_Direct3DDevice;

namespace ts {
  namespace dx {

    //�P���_�ɕK�v�ȃo�C�g�����v�Z����T�[�r�X�֐�
//
//
DWORD	CalcVertexDataSize(DWORD dwFVF)
{
	static const DWORD dwTexCodSize[4] = {2,3,4,1};

	//�P���_�̃f�[�^�T�C�Y
	DWORD dwStride		= 0;
	if (dwFVF & D3DFVF_RESERVED0) {
		dwStride += sizeof(DWORD);
	}
	if ((dwFVF & 0xf0) & D3DFVF_RESERVED1) {
		dwStride += sizeof(DWORD);
	}
	if ((dwFVF & 0xf0) & D3DFVF_DIFFUSE) {
		dwStride += sizeof(DWORD);
	}
	if ((dwFVF & 0xf0) & D3DFVF_NORMAL) {
		dwStride += sizeof(float)*3;
	}
	if ((dwFVF & 0xf0) & D3DFVF_SPECULAR) {
		dwStride += sizeof(DWORD);
	}
	switch(dwFVF & D3DFVF_POSITION_MASK) {
	case D3DFVF_XYZ:
		dwStride += sizeof(float)*3;
		break;
	case D3DFVF_XYZRHW:
		dwStride += sizeof(float)*4;
		break;
	case D3DFVF_XYZB1:
		dwStride += sizeof(float)*(3+1);
		break;
	case D3DFVF_XYZB2:
		dwStride += sizeof(float)*(3+2);
		break;
	case D3DFVF_XYZB3:
		dwStride += sizeof(float)*(3+3);
		break;
	case D3DFVF_XYZB4:
		dwStride += sizeof(float)*(3+4);
		break;
	case D3DFVF_XYZB5:
		dwStride += sizeof(float)*(3+5);
		break;
	}

	//���16bit -> �σe�N�X�`���[
	DWORD	tc = ((dwFVF & D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	if (tc) {
		for( DWORD i=0; i<tc; i++) {
			dwStride += sizeof(float)*dwTexCodSize[dwFVF >> (i*2+16)];
		}
	}

	return dwStride;
}


//�o�[�e�b�N�X�o�b�t�@�[
//
//
//


DX3DVertexBuffer::DX3DVertexBuffer()
{
	m_vbuffer = NULL;

	m_dwFVF = 0;
	m_dwNumVertices = 0;
}


DX3DVertexBuffer::~DX3DVertexBuffer()
{
	SAFE_RELEASE(m_vbuffer);
}



//�o�[�e�b�N�X�o�b�t�@�[�𐶐�����
//
//in
//	dwFVF			�o�[�e�b�N�X�o�b�t�@�[�ɐݒ肷�钸�_�t�H�[�}�b�g
//	dwNumVertices	���_�̌�
//					D3DMAXNUMVERTICES = �S���m��
HRESULT	DX3DVertexBuffer::create(DWORD dwFVF, DWORD dwNumVertices)
{
	D3DDEVICEDESC7 ddDesc;
	if (FAILED( _Direct3DDevice->GetCaps( &ddDesc ) ))
		return E_FAIL;

	D3DVERTEXBUFFERDESC vbdesc;
	ZeroMemory(&vbdesc, sizeof(D3DVERTEXBUFFERDESC));
	vbdesc.dwSize			= sizeof(D3DVERTEXBUFFERDESC);
	vbdesc.dwCaps			= 0L;
	vbdesc.dwFVF			= dwFVF;
	vbdesc.dwNumVertices	= dwNumVertices;

	m_dwFVF			= dwFVF;
	m_dwNumVertices = dwNumVertices;
	

	if (!(ddDesc.dwDevCaps & D3DDEVCAPS_TLVERTEXVIDEOMEMORY))
		vbdesc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
	
	//�o�[�e�b�N�X�o�b�t�@�[�𐶐�
	//
	HRESULT	hr;
	hr = _Direct3D->CreateVertexBuffer(&vbdesc, &m_vbuffer, 0);
	if (FAILED(hr))
		return E_FAIL;

	//�P���_�̃f�[�^�T�C�Y
	m_dwStride = CalcVertexDataSize( m_dwFVF );

	return S_OK;
}



//�o�[�e�b�N�X�o�b�t�@�[�̃f�o�C�X�����擾
//
//
HRESULT	DX3DVertexBuffer::getCaps(D3DVERTEXBUFFERDESC * vbdesc)
{
	return m_vbuffer->GetVertexBufferDesc( vbdesc );
}




//���_�f�[�^���Z�b�g
//
//
HRESULT	DX3DVertexBuffer::setVertex(LPVOID vertex, DWORD num)
{
	HRESULT	hr;
	LPBYTE	lpBuffer;

	hr = m_vbuffer->Lock( DDLOCK_WAIT, (VOID**)&lpBuffer, NULL);
	if (FAILED(hr))
		return hr;

	memcpy( lpBuffer, vertex, num*m_dwStride);

	m_vbuffer->Unlock();

	return S_OK;
}


//LOCK
//
HRESULT	DX3DVertexBuffer::lock( DWORD dwFlags, LPVOID * lpVertex, LPDWORD lpdwSize)
{
	return m_vbuffer->Lock( dwFlags, lpVertex, lpdwSize);
}




//UNLOCK
//
HRESULT	DX3DVertexBuffer::unlock()
{
	return m_vbuffer->Unlock();
}




//DrawPrimitive
//
HRESULT	DX3DVertexBuffer::drawPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType)
{
#if	0
	return _Direct3DDevice->DrawPrimitiveVB(
					d3dptPrimitiveType, m_vbuffer, 
					0, m_dwNumVertices, D3DDP_WAIT);
#else
	return _Direct3DDevice->DrawPrimitiveVB(
					d3dptPrimitiveType, m_vbuffer, 
					0, m_dwNumVertices, 0);
#endif
}



//DrawIndexedPrimitive
//
HRESULT	DX3DVertexBuffer::drawIndexedPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType, 
					       LPWORD lpwIndices, 
					       DWORD dwIndexCount)
{
#if 0
  // _DEBUG
	return _Direct3DDevice->DrawIndexedPrimitiveVB(
					d3dptPrimitiveType, m_vbuffer, 
					0, m_dwNumVertices, 
					lpwIndices, dwIndexCount, D3DDP_WAIT);
#else
	return _Direct3DDevice->DrawIndexedPrimitiveVB(
					d3dptPrimitiveType, m_vbuffer, 
					0, m_dwNumVertices, 
					lpwIndices, dwIndexCount, 0);
#endif
}



//�œK����Ԃɕϊ�
//
//���ӁF�ϊ���̓��b�N�ł��Ȃ��Ȃ�
//�o�[�e�b�N�X�𓮓I�ɕҏW����K�v���Ȃ��Ƃ���
//�œK����Ԃɂ���Ǝ��s���������P�����
HRESULT	DX3DVertexBuffer::optimize()
{
	return m_vbuffer->Optimize( _Direct3DDevice, 0);
}



  }
}