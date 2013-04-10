// test_win32.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "test_win32.h"
#include "MainFrm.h"
#include "libTS/Image/image.hpp"
#include "libTS/Image/extention/extention_factory.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace ts;

// Ctest_win32App

BEGIN_MESSAGE_MAP(Ctest_win32App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &Ctest_win32App::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW_FRAME, &Ctest_win32App::OnFileNewFrame)
    ON_COMMAND(ID_MENU_JPEG, &Ctest_win32App::OnMenuJpeg)
END_MESSAGE_MAP()


// Ctest_win32App コンストラクション

Ctest_win32App::Ctest_win32App()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の Ctest_win32App オブジェクトです。

Ctest_win32App theApp;


// Ctest_win32App 初期化

BOOL Ctest_win32App::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));
	// メイン ウィンドウを作成するとき、このコードは新しいフレーム ウィンドウ オブジェクトを作成し、
	// それをアプリケーションのメイン ウィンドウにセットします
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// メイン MDI フレーム ウィンドウを作成します。
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// 共通の MDI メニューとアクセラレータ テーブルを読み込みます。
	//TODO: 追加のメンバ変数を加えて、アプリケーションが必要とする
	//	追加のメニュータイプのために呼び出しを読み込んでください。
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_test_win32TYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_test_win32TYPE));






	// メイン ウィンドウが初期化されたので、表示と更新を行います。
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// 接尾辞が存在する場合にのみ DragAcceptFiles を呼び出してください。
	//  SDI アプリケーションでは、ProcessShellCommand の直後にこの呼び出しが発生しなければなりません。
	return TRUE;
}


// Ctest_win32App メッセージ ハンドラ




// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void Ctest_win32App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ctest_win32App メッセージ ハンドラ

void Ctest_win32App::OnFileNewFrame() 
{
	CMainFrame* pFrame = new CMainFrame;
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, 
					  NULL, NULL);
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	m_aryFrames.Add(pFrame->GetSafeHwnd());
}

void Ctest_win32App::OnMenuJpeg()
{
  ts::image::basic_image<image::rgba8_pixel_t> jpeg;
  jpeg.loadFromFile("sample1.jpg");
}
