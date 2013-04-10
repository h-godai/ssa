// SystemInterface.cpp
// �v���b�g�t�H�[���Ɉˑ������e�폈��
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



    // SystemInterface�̎���
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

        // �t�@�C���ۑ��_�C�A���O���J��
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

        // �t�@�C���ǂݍ��݃_�C�A���O
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

        // �X�e�[�^�X���b�Z�[�W��\������
        virtual void setStatusMessage(const mstring& msg) {
        	CFrameWnd* pFrame = STATIC_DOWNCAST(CFrameWnd, AfxGetApp()->GetMainWnd());
            if (pFrame != 0) {
              pFrame->SetMessageText(msg.getLPCTSTR());
            }
        }
        // �}�E�X�J�[�\���̐ݒ�
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
          memset(&info, 0, sizeof(info)); // �N���A
          //typedef struct _MEMORYSTATUSEX {
          //  DWORD dwLength;                     // �\���̂̃T�C�Y
          //  DWORD dwMemoryLoad;                 // �������̎g�p��(%)
          //  DWORDLONG ullTotalPhys;             // �����������̓��ڗe��
          //  DWORDLONG ullAvailPhys;             // �����������̋󂫗e��
          //  DWORDLONG ullTotalPageFile;         // �y�[�W���O�̓��ڗe��
          //  DWORDLONG ullAvailPageFile;         // �y�[�W���O�̋󂫗e��
          //  DWORDLONG ullTotalVirtual;          // ���z�������̓��ڗe��
          //  DWORDLONG ullAvailVirtual;          // ���z�������̋󂫗e��
          //  DWORDLONG ullAvailExtendedVirtual;  // ���z�������̊g���󂫗e��
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
          //// �\����
          //typedef struct _PROCESS_MEMORY_COUNTERS {
          //DWORD   cb;                             // �\���̂̃T�C�Y
          //DWORD   PageFaultCount;                 // �y�[�W�E�t�H�[���g��
          //SIZE_T  PeakWorkingSetSize;             // �ő像�[�L���O�Z�b�g
          //SIZE_T  WorkingSetSize;                 // ���݃��[�L���O�Z�b�g
          //SIZE_T  QuotaPeakPagedPoolUsage;        // �ő�y�[�W�v�[���̎g�p�T�C�Y
          //SIZE_T  QuotaPagedPoolUsage;            // ���݃y�[�W�v�[���̎g�p�T�C�Y
          //SIZE_T  QuotaPeakNonPagedPoolUsage;     // �ő��y�[�W�v�[���̎g�p�T�C�Y
          //SIZE_T  QuotaNonPagedPoolUsage;         // ���ݔ�y�[�W�v�[���̎g�p�T�C�Y
          //SIZE_T  PagefileUsage;                  // ���݃y�[�W���O�t�@�C���̎g�p�T�C�Y
          //SIZE_T  PeakPagefileUsage;              // �ő�y�[�W���O�t�@�C���̎g�p�T�C�Y
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
