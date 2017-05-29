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
	OutputDebugStringA(msg.c_str());
	
  }
  virtual void setResultMsg(const std::string& msg) {
    resultMessages_ += msg;
  }
  virtual void setResultMsg(int num, const string& msg, const string& subtitle) {
	string m2(str(boost::format("RESULT#%d: %s TEST:%s\n") % num % msg % subtitle));
	fprintf(stderr, "%s", m2.c_str());
	resultMessages_ += m2;
	OutputDebugStringA(m2.c_str());
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

  void showResultMessages(const string& tn) {
	fprintf(stderr, "--- ALL TEST FINISHED! ---\n*** Result List [%s] ***\n%s\n", tn.c_str(), resultMessages_.c_str());
	OutputDebugStringA("--- ALL TEST FINISHED! ---\r\n*** Result List [");
	OutputDebugStringA( tn.c_str());
	OutputDebugStringA("] ***\r\n");
	OutputDebugStringA(resultMessages_.c_str());
	OutputDebugStringA("\r\n");
  }


  std::vector<std::string>& getTestNames() { return m_testName; }
  typedef std::vector<std::string>::iterator nameListIterator;

private:
  ITestFormEvent::ptr m_testEvent;
  std::vector<std::string> m_testName;
  string resultMessages_;

};
} // namespace
