#ifndef SYS_ISYSTEMINTERFACE_H
#define SYS_ISYSTEMINTERFACE_H


#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#include <string>
#endif

#include "Common/Common.h"
#include "Util/ustring.hpp"

namespace ts {
namespace sys {
  using boost::shared_ptr;
  using std::string;

  enum DialogType {
      DLG_OK,              // OK�̂�
      DLG_OK_CANCEL,            // OK,CANCEL�{�^��
      DLG_YES_NO,               // Yes,No�{�^��
      DLG_YES_NO_CANCEL,        // Yes,No,Cancel�{�^��
      DLG_STYLE_MASK = 0x0ffff, // �X�^�C���̃}�X�N
      DLG_INFOMATION = 0x10000, // ��񃁃b�Z�[�W
      DLG_QUESTION   = 0x20000, // ���⃁�b�Z�[�W
      DLG_WARNING    = 0x30000  // �x�����b�Z�[�W
  };

  enum DialogResult {
      RESULT_YES,     // yes or OK
      RESULT_OK,      // yes or OK
      RESULT_NO,      // no
      RESULT_CANCEL,  // cancel
      RESULT_IGNORE   // ����
  };
  // �}�E�X�J�[�\���̌`��
  enum MouseCursorType {
      ARROW_CURSOR,
      WAIT_CURSOR,
      HAND_CURSOR,
      MOVE_CURSOR,
      TARGET_CURSOR,
      LEFT_MOVE_CURSOR,
      RIGHT_MVOE_CURSOR,
      INVALID_CURSOR
  };

  struct MemoryInfomation {
    uint64_t totalPhys; // �ő啨��������
    uint64_t usedPhys;  // �g�p����������
    uint64_t totalVirtual; // �ő剼�z������
    uint64_t usedVirtual;  // �g�p���z������
    uint64_t processPhys; // �v���Z�X���g�p���Ă��镨��������
    uint64_t processVirtual; // �v���Z�X���g�p���Ă��鉼�z������
  };


  class ISystemInterface {
  public:
    typedef shared_ptr<ISystemInterface> ptr;


    virtual ~ISystemInterface() {}

    // ���I�V���O���g���A�N�Z�X�p
    static ptr GetInstance();


    // ���b�Z�[�W�_�C�A���O��\������
    virtual DialogResult showDialog(const mstring& msg, uint32_t style = DLG_WARNING|DLG_OK) = 0;

    // �t�@�C���ۑ��_�C�A���O
    virtual DialogResult fileSaveDialog(mstring& filename) = 0;

    // �t�@�C���ǂݍ��݃_�C�A���O
    virtual DialogResult fileLoadDialog(mstring& filename) = 0;

    // �X�e�[�^�X���b�Z�[�W��\������
    virtual void setStatusMessage(const mstring& msg) = 0;
    
    // �}�E�X�J�[�\���̐ݒ�
    virtual MouseCursorType setMouseCursor(MouseCursorType)  = 0;

    // �}�E�X�J�[�\���̐ݒ�(�ȑO�̏�Ԃ�ۑ�)
    virtual void pushMouseCursor(MouseCursorType type = INVALID_CURSOR)  = 0;

    // �}�E�X�J�[�\���̐ݒ�
    virtual void popMouseCursor()  = 0;

    // �}�E�X�J�[�\���̎擾
    virtual MouseCursorType getCurrentMouseCursor() const = 0;

    // ���������̎擾
    virtual bool getMemoryInformation(MemoryInfomation& info, uint32_t processId = 0) const = 0;


  };

  // �}�N��
  inline bool ShowYesNoDialog(const mstring& msg) {
    return ISystemInterface::GetInstance()->showDialog(msg, DLG_QUESTION | DLG_YES_NO) == RESULT_YES;
  }
  inline void ShowWarningDialog(const mstring& msg) {
    ISystemInterface::GetInstance()->showDialog(msg, DLG_WARNING | DLG_OK);
  }
  inline void SetStatusMessage(const mstring& msg) {
    ISystemInterface::GetInstance()->setStatusMessage(msg);
  }
  inline MouseCursorType SetMouseCursor(MouseCursorType type) {
    return ISystemInterface::GetInstance()->setMouseCursor(type);
  }
  inline void PushMouseCursor(MouseCursorType type) {
    ISystemInterface::GetInstance()->pushMouseCursor(type);
  }
  inline void PopMouseCursor() {
    ISystemInterface::GetInstance()->popMouseCursor();
  }
  inline bool getMemoryInformation(MemoryInfomation& info, uint32_t processId = 0) {
    return ISystemInterface::GetInstance()->getMemoryInformation(info, processId);
  }


}
}

#endif
