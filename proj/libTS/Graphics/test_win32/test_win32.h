// test_win32.h : test_win32 アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// Ctest_win32App:
// このクラスの実装については、test_win32.cpp を参照してください。
//

class Ctest_win32App : public CWinApp
{
public:
	Ctest_win32App();

	CArray<HWND, HWND> m_aryFrames;
public:

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装
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