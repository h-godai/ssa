// ITestBody.h
#pragma once

#include <boost/shared_ptr.hpp>
#include "ITestForm.h"

namespace app {
  using boost::shared_ptr;

  class ITestBody {
  public:
    typedef shared_ptr<ITestBody> ptr;

    virtual ~ITestBody() {}

    // テスト名
    virtual const char* getTestName() const = 0;

    // テストプログラムの個数を返す
    virtual int getTestCount() const = 0;

    // テストのサブタイトルを得る
    virtual const char* getSubTitle(int) {
      return "no title";
    }
    
    // テストの実行
    double doTest(int testNum, int sequence, ITestForm::ptr testForm) {
      testForm_ = testForm;
      return doTestImpl(testNum, sequence);
    }
    // 1テストあたりのループ数を返す
    virtual size_t getTestLoops(int) const { return 1; }

  private:
    // テスト実行の実装
    virtual double doTestImpl(int testNum, int sequence) = 0;

    // 継承クラス用のサービスルーチン
  protected:
    // メッセージ出力
    void outputMessage(const string& msg) {
      if (testForm_) {
        testForm_->outputMessage(msg);
      }
    }

    ITestForm::ptr testForm_;
  

  };



} // namespace



