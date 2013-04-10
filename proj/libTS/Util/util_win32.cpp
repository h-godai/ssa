// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
#endif

// ���Ŏw�肳�ꂽ��`�̑O�ɑΏۃv���b�g�t�H�[�����w�肵�Ȃ���΂Ȃ�Ȃ��ꍇ�A�ȉ��̒�`��ύX���Ă��������B
// �قȂ�v���b�g�t�H�[���ɑΉ�����l�Ɋւ���ŐV���ɂ��ẮAMSDN ���Q�Ƃ��Ă��������B
#ifndef WINVER				// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define WINVER 0x0501		// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINNT		// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B                   
#define _WIN32_WINNT 0x0501	// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_WINDOWS 0x0410 // ����� Windows Me �܂��͂���ȍ~�̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_IE			// IE 6.0 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_IE 0x0600	// ����� IE �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// �ꕔ�� CString �R���X�g���N�^�͖����I�ł��B

#ifdef _AFXDLL
// ��ʓI�Ŗ������Ă����S�� MFC �̌x�����b�Z�[�W�̈ꕔ�̔�\�����������܂��B
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �̃R�A����ѕW���R���|�[�l���g
#include <afxext.h>         // MFC �̊g������

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �R���� �R���g���[�� �T�|�[�g
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �R���� �R���g���[�� �T�|�[�g
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

// �t�H���_�I���_�C�A���O��\������D
ustring util::selectDirectory(const ustring& initialDirectory) {
  ustring result(initialDirectory);
  BROWSEINFO bInfo;
  LPITEMIDLIST pIDList;
  TCHAR szDisplayName[MAX_PATH];
  
  // BROWSEINFO�\���̂ɒl��ݒ�
  bInfo.hwndOwner             = AfxGetMainWnd()->m_hWnd;          // �_�C�A���O�̐e�E�C���h�E�̃n���h��
  bInfo.pidlRoot              = NULL;                             // ���[�g�t�H���_������ITEMIDLIST�̃|�C���^ (NULL�̏ꍇ�f�X�N�g�b�v�t�H���_���g���܂��j
  bInfo.pszDisplayName        = szDisplayName;                    // �I�����ꂽ�t�H���_�����󂯎��o�b�t�@�̃|�C���^
  bInfo.lpszTitle             = _T("�t�H���_�̑I��");             // �c���[�r���[�̏㕔�ɕ\������镶���� 
  bInfo.ulFlags               = BIF_RETURNONLYFSDIRS;             // �\�������t�H���_�̎�ނ������t���O
  bInfo.lpfn = callback; // �R�[���o�b�N�֐��̃A�h���X��ݒ�
#ifdef _UNICODE
  bInfo.lParam = (LPARAM)initialDirectory.c_str();  // �w�肵�����p�X��ݒ�
#else
  bInfo.lParam = (LPARAM)initialDirectory.toChar();  // �w�肵�����p�X��ݒ�
#endif

  // �t�H���_�I���_�C�A���O��\��
  pIDList = ::SHBrowseForFolder(&bInfo);
  if(pIDList == NULL){

      // �߂�l��NULL�̏ꍇ�A�t�H���_���I�����ꂸ�Ƀ_�C�A���O������ꂽ���Ƃ��Ӗ����܂��B

  }else{

      // ItemIDList���p�X���ɕϊ����܂�
      if(!::SHGetPathFromIDList(pIDList, szDisplayName)){
          // �G���[����
      }
      else {
      // szDisplayName�ɑI�����ꂽ�t�H���_�̃p�X�������Ă��܂�
        result = szDisplayName;
      }


      // �Ō��pIDList�̃|�C���g���Ă��郁�������J�����܂�
      ::CoTaskMemFree( pIDList );
  }
  return result;
}

#endif

} // namespace
