// PerformanceTestForm.h
//
// PerformanceTestのユーザーインターフェイス

#pragma once
#include <string>
#include <vector>
#include "ITestForm.h"

using app::ITestForm;
using app::ITestFormEvent;

class PerformanceTestDlg;

class PerformanceTestForm : public ITestForm {
public:
  PerformanceTestForm(PerformanceTestDlg& parent);
  virtual  ~PerformanceTestForm(void) {}

  virtual void setEvent(ITestFormEvent::ptr testEvent) {
    m_testEvent = testEvent;
  }
  virtual void outputMessage(const std::string& msg);
  virtual void setResultMsg(int num, const std::string& msg);
  virtual void addTestName(const std::string& name) {
    m_testName.push_back(name);
  }

  void startTest(int kind, int num) {
    if (m_testEvent.get() != 0) {
      m_testEvent->startTestEvent(kind, num);
    }
  }

  std::vector<std::string>& getTestNames() { return m_testName; }
  typedef std::vector<std::string>::iterator nameListIterator;

private:
  ITestFormEvent::ptr m_testEvent;
  PerformanceTestDlg& m_parent;
  std::vector<std::string> m_testName;

};
