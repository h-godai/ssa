// PerformanceTestForm.h
//
// PerformanceTestのユーザーインターフェイス

#pragma once
#include <string>
#include <vector>
#include "ITestForm.h"

namespace app {
  using app::ITestForm;
  using app::ITestFormEvent;


class PerformanceTestForm : public ITestForm {
public:
	PerformanceTestForm() {};
	virtual  ~PerformanceTestForm(void) {}

  virtual void setEvent(ITestFormEvent::ptr testEvent) {
    m_testEvent = testEvent;
  }
  virtual void outputMessage(const std::string& msg) {
    fprintf(stderr, " %s",  msg.c_str());

  }
  virtual void setResultMsg(int num, const std::string& msg) {
    fprintf(stderr, "RESULT#%d: %s\n", num, msg.c_str());
  }
  virtual void addTestName(const std::string& name) {
    m_testName.push_back(name);
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
  void saveResult(const std::string& fn) {
    if (m_testEvent) {
      m_testEvent->saveResult(fn);
    }
  }


  std::vector<std::string>& getTestNames() { return m_testName; }
  typedef std::vector<std::string>::iterator nameListIterator;

private:
  ITestFormEvent::ptr m_testEvent;
  std::vector<std::string> m_testName;

};
} // namespace
