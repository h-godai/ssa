// test_win32.h : test_win32 �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"       // ���C�� �V���{��


// Ctest_win32App:
// ���̃N���X�̎����ɂ��ẮAtest_win32.cpp ���Q�Ƃ��Ă��������B
//

class Ctest_win32App : public CWinApp
{
public:
	Ctest_win32App();

	CArray<HWND, HWND> m_aryFrames;
public:

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNewFrame();
	DECLARE_MESSAGE_MAP()
    afx_msg void OnMenuJpeg();
};

extern Ctest_win32App theApp;