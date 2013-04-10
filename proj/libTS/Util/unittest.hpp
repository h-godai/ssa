/*!	@file	unittest.hpp -*-tab-width:4-*-
	@brief 単体テスト用クラス
	@author	ikeda
	@note	unittest.hppはC++専用の単体試験用クラスです。
*/

#ifndef TSL_UNITTEST_HPP
#define TSL_UNITTEST_HPP

#ifndef STDAFX_H
#include <sstream>
#include <vector>
#include <string>
#endif

namespace ts {
  
  /*!
	@class	Unittest
	@brief	単体試験用クラス
	@note	クラスの単体試験を効率よく行うためのサービスクラスです。<br>
	POINTCUT_UNITTEST_METHOD をクラス定義に追加<br>
	bool unittest(Unittest*) メソッドの実装(テスト実施)<br>
	unittest()内で、checkメソッドを用いてテスト結果を報告します。<br>
	POINTCUT_UNITTESET_REGIST(<classname>)の実装<br>
	を行った後、Unittest::run()によってすべての単体試験を行います。<br>
  */
  class Unittest {
  public:
	//!	@brief テスト実施用メソッドの型定義
	typedef bool (*TestProc)(Unittest*);
	typedef void (*OutputMsg)(const std::string&);
	
	//! @brief コンストラクタ
	Unittest()
	  : m_count(0)
	  , m_success(0)
	  , m_silent(false)
	  , m_outmsg(DefaultOutMsg)
	  , m_outerrmsg(DefaultOutErrMsg)
	{}
	
	//! @brief シングルトン取得用メソッド
	static Unittest& Instance(); // singleton

	static void DefaultOutMsg(const std::string& msg);
	static void DefaultOutErrMsg(const std::string& msg);

	
	//! @brief テストメソッド追加用メソッド(シングルトン使用)
	static void append(Unittest::TestProc proc);
	
	//! @brief テストメソッド追加ようオペレータ
	void operator += (TestProc proc) {
	  m_testList.push_back(proc);
	}
	
	//! @brief テスト実施
	bool run(const std::string& testname="");
	static bool Run(const std::string& testname="") { return Instance().run(testname); }
	
	//! @brief テストメソッドから呼ばれるテスト結果報告用メソッド
	bool check(bool chk, const std::string& prefix, const char* comment=0);
	
	//! @brief テストログの取得
	std::string getLog() { return m_log.str(); }
	
    //! @brief メッセージ出力
    void message(const std::string& msg);

    //! @brief スタートメッセージ出力
    void startMessage(const std::string& msg);
	
    void setSilentMode(bool enable = true) { m_silent = enable; }
	
    Unittest& operator << (const std::string& msg) {
      message(msg);
      return *this;
    }
	
  private:
	int m_count;
	int m_success;
	std::vector <TestProc> m_testList;
	std::stringstream m_log;
	std::stringstream m_errlog;
    bool m_silent;
	OutputMsg m_outmsg;
	OutputMsg m_outerrmsg;
	std::string m_testname;
  };
}

// ポイントカット用のunittestメソッドの定義用マクロ
#define POINTCUT_UNITTEST_METHOD static bool unittest(ts::Unittest* = 0);

// ポイントカット登録用マクロ!!
#define POINTCUT_UNITTEST_REGIST(cls) static struct unittestRegist##cls { \
	unittestRegist##cls() { ts::Unittest::append(cls::unittest); } } globalRegist##cls;




#endif
