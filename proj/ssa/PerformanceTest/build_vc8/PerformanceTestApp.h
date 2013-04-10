// PerformanceTest.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "TestMain.h"
using app::TestMain;

// PerformanceTestApp:
// このクラスの実装については、PerformanceTest.cpp を参照してください。
//

class PerformanceTestApp : public CWinApp
{
public:
  PerformanceTestApp();

// オーバーライド
public:
  virtual BOOL InitInstance();

// 実装

  DECLARE_MESSAGE_MAP()


};

extern PerformanceTestApp theApp;