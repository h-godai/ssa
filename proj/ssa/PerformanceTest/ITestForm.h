
// ITestForm.h
//
// PerformanceTestのユーザーインターフェイスを定義する仮想クラス

#ifndef APP_ITESTFORM_H
#define APP_ITESTFORM_H

#include <string>
#include <boost/shared_ptr.hpp>

#ifdef _MSC_VER
#define ENDL "\r\n" 
#else
#define ENDL "\n"
#endif



namespace app {

  using boost::shared_ptr;
  using std::string;


  class ITestFormEvent {
  public:
    typedef shared_ptr<ITestFormEvent> ptr;

    ITestFormEvent() {}
    virtual ~ITestFormEvent() {}

    // テスト開始
    virtual void startTestEvent(int kind, int num) = 0;

    // リセット
    virtual void reset() = 0;
    // 結果保存
    virtual void saveResult(const std::string&) = 0;
  };

  class ITestForm {
  public:
    typedef shared_ptr<ITestForm> ptr;

    ITestForm() {}
    virtual ~ITestForm() {}

    // ITestFormを継承したクラスを取得する
    static ptr getInstance();

    static bool isNull(ITestForm::ptr p) {
      return p.get() == 0;
    }

    virtual void setResultMsg(const std::string& msg)  = 0;
    virtual void setEvent(ITestFormEvent::ptr event) = 0;
    virtual void outputMessage(const string& msg) = 0;
    virtual void setResultMsg(int num, const string& msg, const string& sub) = 0;
    virtual void addTestName(const string& name) = 0;
	virtual void showResultMessages(const string& msg) {}

  };




}

#endif
