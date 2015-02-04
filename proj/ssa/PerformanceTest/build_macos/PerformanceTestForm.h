// PerformanceTestForm.h
//
// PerformanceTestのユーザーインターフェイス

#pragma once
#include <string>
#include <vector>
#include "ITestForm.h"
#include <boost/format.hpp>

namespace app {
  using app::ITestForm;
  using app::ITestFormEvent;
  using std::string;
  
  class PerformanceTestForm : public ITestForm {
  public:
	PerformanceTestForm() {};
	virtual  ~PerformanceTestForm(void) {}

  virtual void setEvent(ITestFormEvent::ptr testEvent) {
    m_testEvent = testEvent;
  }
  virtual void outputMessage(const string& msg) {
    fprintf(stderr, " %s",  msg.c_str());

  }
  virtual void setResultMsg(int num, const string& msg, const string& subtitle) {
	string m2(str(boost::format("RESULT#%d: %s TEST:%s\n") % num % msg % subtitle));
	fprintf(stderr, "%s", m2.c_str());
	resultMessages_ += m2;
  }
  virtual void addTestName(const string& name) {
    testName_.push_back(name);
  }

  void startTest(int kind, int num) {
    if (m_testEvent) {
      m_testEvent->startTestEvent(kind, num);
    }
  }
  void reset() {
    if (m_testEvent) {
      m_testEvent->reset();
    }
  }
  void saveResult(const string& fn) {
    if (m_testEvent) {
      m_testEvent->saveResult(fn);
    }
  }

  void showResultMessages(const string& tn) {
	fprintf(stderr, "--- ALL TEST FINISHED! ---\n*** Result List [%s] ***\n%s\n", tn.c_str(), resultMessages_.c_str());
  }

  std::vector<string>& getTestNames() { return testName_; }
  typedef std::vector<string>::iterator nameListIterator;

private:
  ITestFormEvent::ptr m_testEvent;
  std::vector<string> testName_;
  string resultMessages_;

};
} // namespace
