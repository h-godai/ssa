// SystemInterface.cpp
// プラットフォームに依存した各種処理
//
// for Windows

#include "stdafx.h"
#include "resource.h"
#include "PlatformDependent/Sys/ISystemInterface.h"
#include "Util/ustring.hpp"
#include "Util/Logger.h"
#include <Psapi.h>      // Psapi.Lib

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <stack>

namespace ts {
namespace sys {
    using std::stack;



    // SystemInterfaceの実装
    class SystemInterface : public ISystemInterface {
    public:
        SystemInterface()
            : m_cursorType(ARROW_CURSOR)
        {
            m_handCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND);
        }
        virtual ~SystemInterface() {
        }
        virtual DialogResult  showDialog(const mstring& msg, uint32_t style) {
            UINT type = MB_OK;
            switch(style & DLG_STYLE_MASK) {
            case DLG_OK:            type = MB_OK ; break;
            case DLG_OK_CANCEL:     type = MB_OKCANCEL ; break;
            case DLG_YES_NO:        type = MB_YESNO; break;
            case DLG_YES_NO_CANCEL: type = MB_YESNOCANCEL; break;
            default: break;
            }
            switch(style & (~DLG_STYLE_MASK)) {
            case DLG_INFOMATION:    type |= MB_ICONINFORMATION; break;
            case DLG_QUESTION:      type |= MB_ICONQUESTION; break;
            case DLG_WARNING:       type |= MB_ICONEXCLAMATION   ; break;
            default: break;
            }
            return convResult(AfxMessageBox(msg.getLPCTSTR(), type));
        }

        // ファイル保存ダイアログを開く
        virtual DialogResult fileSaveDialog(mstring& filename) {
            const int bufferSize = 4096;
            CString reg;
            reg = AfxGetApp()->GetProfileString(_T("PROJECT"), _T("DIR"), _T("."));

            CFileDialog dlgFile(FALSE, _T("imc"), filename.getLPCTSTR(),
                                OFN_NOCHANGEDIR|OFN_ENABLESIZING|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                                _T("IMC AuthoringTool file(*.imc)|*.imc|All Files(*.*)|(*.*)||"), NULL);
            CString fileName(filename.c_str());
            dlgFile.GetOFN().lpstrInitialDir = reg;
            dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(bufferSize);
            dlgFile.GetOFN().nMaxFile = bufferSize;
            if (dlgFile.DoModal() != IDOK) { return RESULT_CANCEL; }

            TCHAR drive[4];
            TCHAR dir[bufferSize];
            TCHAR fn[bufferSize];
            TCHAR ext[256];
            ::_tsplitpath_s(fileName.GetBuffer(), drive, dir, fn, ext);
            CString newDir(drive);
            newDir += dir;
            AfxGetApp()->WriteProfileString(_T("PROJECT"), _T("DIR"), newDir.GetBuffer());
            filename = fileName.GetBuffer();
            return RESULT_OK;
        }

        // ファイル読み込みダイアログ
        virtual DialogResult fileLoadDialog(mstring& filename) {
            const int bufferSize = 4096;
            CString reg;
            reg = AfxGetApp()->GetProfileString(_T("PROJECT"), _T("DIR"), _T("."));

            CFileDialog dlgFile(TRUE, _T("imc"), NULL,
                                OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST|OFN_ENABLESIZING,
                                _T("IMC AuthoringTool file(*.imc)|*.imc|All Files(*.*)|(*.*)||"), NULL);
            CString fileName;
            dlgFile.GetOFN().lpstrInitialDir = reg;
            dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(bufferSize);
            dlgFile.GetOFN().nMaxFile = bufferSize;
            if (dlgFile.DoModal() != IDOK) { return RESULT_CANCEL; }

            TCHAR drive[4];
            TCHAR dir[bufferSize];
            TCHAR fn[bufferSize];
            TCHAR ext[256];
            ::_tsplitpath_s(fileName.GetBuffer(), drive, dir, fn, ext);
            CString newDir(drive);
            newDir += dir;
            AfxGetApp()->WriteProfileString(_T("PROJECT"), _T("DIR"), newDir.GetBuffer());
            filename = fileName.GetBuffer();
            return RESULT_OK;
        }

        // ステータスメッセージを表示する
        virtual void setStatusMessage(const mstring& msg) {
        	CFrameWnd* pFrame = STATIC_DOWNCAST(CFrameWnd, AfxGetApp()->GetMainWnd());
            if (pFrame != 0) {
              pFrame->SetMessageText(msg.getLPCTSTR());
            }
        }
        // マウスカーソルの設定
        virtual MouseCursorType setMouseCursor(MouseCursorType type) {
            switch(type) {
            case ARROW_CURSOR:  ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));  break;
            case WAIT_CURSOR:   ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));   break;
            case HAND_CURSOR:   ::SetCursor(m_handCursor);   break;
            case MOVE_CURSOR:   ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));   break;
            case TARGET_CURSOR: ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));   break;
            case LEFT_MOVE_CURSOR:  ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));   break;
            case RIGHT_MVOE_CURSOR: ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));   break;
            default:
                break;
            }
            MouseCursorType oldType = m_cursorType;
            m_cursorType = type;
            return oldType;
        }

        virtual void pushMouseCursor(MouseCursorType type = INVALID_CURSOR) {
            m_cursorStack.push(m_cursorType);
            if (type != INVALID_CURSOR) {
                SetMouseCursor(type);
            }
        }

        virtual void popMouseCursor() {
            if (m_cursorStack.size() > 0) {
                SetMouseCursor(m_cursorStack.top());
                m_cursorStack.pop();
            }
        }

        virtual MouseCursorType getCurrentMouseCursor() const {
            return m_cursorType;
        }


        virtual bool getMemoryInformation(MemoryInfomation& info, uint32_t processId = 0) const {
          memset(&info, 0, sizeof(info)); // クリア
          //typedef struct _MEMORYSTATUSEX {
          //  DWORD dwLength;                     // 構造体のサイズ
          //  DWORD dwMemoryLoad;                 // メモリの使用率(%)
          //  DWORDLONG ullTotalPhys;             // 物理メモリの搭載容量
          //  DWORDLONG ullAvailPhys;             // 物理メモリの空き容量
          //  DWORDLONG ullTotalPageFile;         // ページングの搭載容量
          //  DWORDLONG ullAvailPageFile;         // ページングの空き容量
          //  DWORDLONG ullTotalVirtual;          // 仮想メモリの搭載容量
          //  DWORDLONG ullAvailVirtual;          // 仮想メモリの空き容量
          //  DWORDLONG ullAvailExtendedVirtual;  // 仮想メモリの拡張空き容量
          //} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
          MEMORYSTATUSEX memstat;
          if (::GlobalMemoryStatusEx(&memstat) != FALSE) {
            info.totalPhys = static_cast<uint64_t>(memstat.ullTotalPhys);
            info.totalVirtual = static_cast<uint64_t>(memstat.ullTotalVirtual);
            info.usedPhys = info.totalPhys - static_cast<uint64_t>(memstat.ullAvailPhys);
            info.usedVirtual = info.totalVirtual - static_cast<uint64_t>(memstat.ullAvailVirtual);
          }
          else {
            MEMORYSTATUS memstat;
            GlobalMemoryStatus(&memstat);
            info.totalPhys = static_cast<uint64_t>(memstat.dwTotalPhys);
            info.totalVirtual = static_cast<uint64_t>(memstat.dwTotalVirtual);
            info.usedPhys = info.totalPhys - static_cast<uint64_t>(memstat.dwAvailPhys);
            info.usedVirtual = info.totalVirtual - static_cast<uint64_t>(memstat.dwAvailVirtual);
          }


          DWORD pid = static_cast<DWORD>(processId);
          HANDLE hProcess = NULL;
          if (processId == 0) {
            pid = ::GetCurrentProcessId();
          }
          //// 構造体
          //typedef struct _PROCESS_MEMORY_COUNTERS {
          //DWORD   cb;                             // 構造体のサイズ
          //DWORD   PageFaultCount;                 // ページ・フォールト数
          //SIZE_T  PeakWorkingSetSize;             // 最大ワーキングセット
          //SIZE_T  WorkingSetSize;                 // 現在ワーキングセット
          //SIZE_T  QuotaPeakPagedPoolUsage;        // 最大ページプールの使用サイズ
          //SIZE_T  QuotaPagedPoolUsage;            // 現在ページプールの使用サイズ
          //SIZE_T  QuotaPeakNonPagedPoolUsage;     // 最大非ページプールの使用サイズ
          //SIZE_T  QuotaNonPagedPoolUsage;         // 現在非ページプールの使用サイズ
          //SIZE_T  PagefileUsage;                  // 現在ページングファイルの使用サイズ
          //SIZE_T  PeakPagefileUsage;              // 最大ページングファイルの使用サイズ
          //} PROCESS_MEMORY_COUNTERS, *PPROCESS_MEMORY_COUNTERS;
          if ((hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid)) != NULL) {
            PROCESS_MEMORY_COUNTERS ppsmemCounters;
            if (::GetProcessMemoryInfo(hProcess, &ppsmemCounters, sizeof(ppsmemCounters)) != FALSE) {
              info.processPhys = static_cast<uint64_t>(ppsmemCounters.WorkingSetSize);
              info.processVirtual = static_cast<uint64_t>(ppsmemCounters.PagefileUsage);
            }
            ::CloseHandle(hProcess);
          }
          return true;
        }



    private:

        DialogResult convResult(INT_PTR  winResult) {
            switch (winResult) {
            case IDYES:    return RESULT_YES;
            case IDOK:     return RESULT_OK;
            case IDNO:     return RESULT_NO;
            case IDCANCEL: return RESULT_CANCEL;
            case IDIGNORE: return RESULT_IGNORE;
            default: return RESULT_YES;
            }
        }

        MouseCursorType m_cursorType;
        stack <MouseCursorType> m_cursorStack;
        HICON m_handCursor;
    };






    ISystemInterface::ptr ISystemInterface::GetInstance() {
        static ISystemInterface::ptr p_myInstance;
        if (p_myInstance.get() == 0) {
          p_myInstance = SystemInterface::ptr(new SystemInterface);
        }
        return p_myInstance;
    }



}
}
