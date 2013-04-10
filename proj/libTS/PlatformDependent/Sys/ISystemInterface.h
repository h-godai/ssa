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
      DLG_OK,              // OKのみ
      DLG_OK_CANCEL,            // OK,CANCELボタン
      DLG_YES_NO,               // Yes,Noボタン
      DLG_YES_NO_CANCEL,        // Yes,No,Cancelボタン
      DLG_STYLE_MASK = 0x0ffff, // スタイルのマスク
      DLG_INFOMATION = 0x10000, // 情報メッセージ
      DLG_QUESTION   = 0x20000, // 質問メッセージ
      DLG_WARNING    = 0x30000  // 警告メッセージ
  };

  enum DialogResult {
      RESULT_YES,     // yes or OK
      RESULT_OK,      // yes or OK
      RESULT_NO,      // no
      RESULT_CANCEL,  // cancel
      RESULT_IGNORE   // 無視
  };
  // マウスカーソルの形状
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
    uint64_t totalPhys; // 最大物理メモリ
    uint64_t usedPhys;  // 使用物理メモリ
    uint64_t totalVirtual; // 最大仮想メモリ
    uint64_t usedVirtual;  // 使用仮想メモリ
    uint64_t processPhys; // プロセスが使用している物理メモリ
    uint64_t processVirtual; // プロセスが使用している仮想メモリ
  };


  class ISystemInterface {
  public:
    typedef shared_ptr<ISystemInterface> ptr;


    virtual ~ISystemInterface() {}

    // 動的シングルトンアクセス用
    static ptr GetInstance();


    // メッセージダイアログを表示する
    virtual DialogResult showDialog(const mstring& msg, uint32_t style = DLG_WARNING|DLG_OK) = 0;

    // ファイル保存ダイアログ
    virtual DialogResult fileSaveDialog(mstring& filename) = 0;

    // ファイル読み込みダイアログ
    virtual DialogResult fileLoadDialog(mstring& filename) = 0;

    // ステータスメッセージを表示する
    virtual void setStatusMessage(const mstring& msg) = 0;
    
    // マウスカーソルの設定
    virtual MouseCursorType setMouseCursor(MouseCursorType)  = 0;

    // マウスカーソルの設定(以前の状態を保存)
    virtual void pushMouseCursor(MouseCursorType type = INVALID_CURSOR)  = 0;

    // マウスカーソルの設定
    virtual void popMouseCursor()  = 0;

    // マウスカーソルの取得
    virtual MouseCursorType getCurrentMouseCursor() const = 0;

    // メモリ情報の取得
    virtual bool getMemoryInformation(MemoryInfomation& info, uint32_t processId = 0) const = 0;


  };

  // マクロ
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
