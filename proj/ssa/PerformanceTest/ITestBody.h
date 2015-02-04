// ITestBody.h -*-c++-*-
#pragma once

#include <string.h>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include "ITestForm.h"

namespace app {
  using boost::shared_ptr;
  using std::string;

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

	bool operator < (const ITestBody& b) const {
	  return string(getTestName()) < string(b.getTestName());
	}
	
	const string& getAdditionalTitles() const { return additionalTitles_; }
	string getAdditionalResult(size_t n) const { 
	  if (n < additionalResults_.size()) return additionalResults_[n];
	  return "";
	}

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

	void setAdditionalTitles(const string& s) { additionalTitles_ = s; }
	void addAdditionalResults(const string& s)  { additionalResults_.push_back(s); }


    ITestForm::ptr testForm_;
	std::string additionalTitles_;
	std::vector<std::string> additionalResults_;
  

  };

  inline bool operator < (const ITestBody::ptr& a, const ITestBody::ptr& b) {
	return *a < *b;
  }

} // namespace



