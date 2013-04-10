// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows ヘッダーから使用されていない部分を除外します。
#endif

// 下で指定された定義の前に対象プラットフォームを指定しなければならない場合、以下の定義を変更してください。
// 異なるプラットフォームに対応する値に関する最新情報については、MSDN を参照してください。
#ifndef WINVER				// Windows XP 以降のバージョンに固有の機能の使用を許可します。
#define WINVER 0x0501		// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_WINDOWS 0x0410 // これを Windows Me またはそれ以降のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_IE			// IE 6.0 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_IE 0x0600	// これを IE の他のバージョン向けに適切な値に変更してください。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 一部の CString コンストラクタは明示的です。

#ifdef _AFXDLL
// 一般的で無視しても安全な MFC の警告メッセージの一部の非表示を解除します。
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#else
#include <windows.h>
#endif


#include "util.hpp"
using namespace std;


namespace {
#ifdef _AFXDLL
  int CALLBACK callback(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData) {
      if(uMsg == BFFM_INITIALIZED) {
          SendMessage(hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
      }
      return 0;
  }
#endif
}

namespace ts {

#ifdef _AFXDLL

// フォルダ選択ダイアログを表示する．
ustring util::selectDirectory(const ustring& initialDirectory) {
  ustring result(initialDirectory);
  BROWSEINFO bInfo;
  LPITEMIDLIST pIDList;
  TCHAR szDisplayName[MAX_PATH];
  
  // BROWSEINFO構造体に値を設定
  bInfo.hwndOwner             = AfxGetMainWnd()->m_hWnd;          // ダイアログの親ウインドウのハンドル
  bInfo.pidlRoot              = NULL;                             // ルートフォルダを示すITEMIDLISTのポインタ (NULLの場合デスクトップフォルダが使われます）
  bInfo.pszDisplayName        = szDisplayName;                    // 選択されたフォルダ名を受け取るバッファのポインタ
  bInfo.lpszTitle             = _T("フォルダの選択");             // ツリービューの上部に表示される文字列 
  bInfo.ulFlags               = BIF_RETURNONLYFSDIRS;             // 表示されるフォルダの種類を示すフラグ
  bInfo.lpfn = callback; // コールバック関数のアドレスを設定
#ifdef _UNICODE
  bInfo.lParam = (LPARAM)initialDirectory.c_str();  // 指定したいパスを設定
#else
  bInfo.lParam = (LPARAM)initialDirectory.toChar();  // 指定したいパスを設定
#endif

  // フォルダ選択ダイアログを表示
  pIDList = ::SHBrowseForFolder(&bInfo);
  if(pIDList == NULL){

      // 戻り値がNULLの場合、フォルダが選択されずにダイアログが閉じられたことを意味します。

  }else{

      // ItemIDListをパス名に変換します
      if(!::SHGetPathFromIDList(pIDList, szDisplayName)){
          // エラー処理
      }
      else {
      // szDisplayNameに選択されたフォルダのパスが入っています
        result = szDisplayName;
      }


      // 最後にpIDListのポイントしているメモリを開放します
      ::CoTaskMemFree( pIDList );
  }
  return result;
}

#endif

} // namespace
