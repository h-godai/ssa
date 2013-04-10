#pragma once

#include "ITestForm.h"
#include "ITestBody.h"
#include <vector>

namespace app {
  using std::vector;

  class TestMain {
  public:
    TestMain(ITestForm::ptr testForm);
    ~TestMain() {
      if (m_testBody != 0) {
        delete m_testBody;
      }
    }

    void doTest(int kind, int num);
    void doAllTest(int kind);
    void reset() {
      m_result1 = 0.0;
      m_allResult.clear();
    }
    void saveResult(const std::string&);


    // メッセージ出力
    void outputMessage(const string& msg) {
      //if (m_testForm.get() != 0) {
      if (!ITestForm::isNull(m_testForm)) {
        m_testForm->outputMessage(msg);
      }
    }

    static volatile void tick(int sum = 1);
    static volatile void tick(void*);

    static void addTestBody(ITestBody::ptr test) {
      if (m_testBody == 0) {
        m_testBody = new vector<ITestBody::ptr>();
      }
      m_testBody->push_back(test);
    }

    static int getTick() { return m_tick; }

  private:
    ITestForm::ptr m_testForm;
    vector<vector<double> > m_allResult;
    double m_result1;
    static volatile int m_tick;
    static vector<ITestBody::ptr>* m_testBody;
    int m_latestKind;

  };



}

