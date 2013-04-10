#include "stdafx.h"
#include "TestMain.h"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::io;



namespace app {

  vector<ITestBody::ptr>* TestMain::m_testBody = 0;


  const char CREDIT_MSG[] = "Performance Test Program Version 0.31 (2012/12/18)" ENDL 
                            "Created by h.godai godai@techarts.co.jp" ENDL  ENDL ;

  class TestEvent : public ITestFormEvent {
  public:
    TestEvent(TestMain& testMain) : m_testMain(testMain) {}
    virtual ~TestEvent() {}
    virtual void startTestEvent(int kind, int num) {
      if (num == 0) {
        // 全テスト開始
        m_testMain.doAllTest(kind);
      }
      else {
        // テスト開始
        m_testMain.doTest(kind, num);
      }
    }
    virtual void reset() {
      m_testMain.reset();
    }
    virtual void saveResult(const std::string& fn) {
      m_testMain.saveResult(fn);
    }

  private:
    TestMain& m_testMain;
  };



  TestMain::TestMain(ITestForm::ptr testForm) 
    : m_testForm(testForm), m_latestKind(0)
  {
    // eventオブジェクトの生成
    ITestFormEvent::ptr testEvent(new TestEvent(*this));

    // eventの登録
    m_testForm->setEvent(testEvent);

    // クレジットの表示
    m_testForm->outputMessage(CREDIT_MSG);

    // テストの登録
    if (m_testBody != 0) {
      for (vector<ITestBody::ptr>::iterator i = m_testBody->begin(); i != m_testBody->end(); ++i) {
        m_testForm->addTestName((*i)->getTestName());
      }
    }
  }

  void TestMain::doAllTest(int kind) {
    if (m_testBody == 0 || m_testBody->size() <= static_cast<size_t>(kind)) {
      outputMessage(str(format("unknown test kind %d" ENDL ) % kind));
      return;
    }
    int numTests = (*m_testBody)[kind]->getTestCount();
    for (int num = 1; num <= numTests; ++num) {
      doTest(kind, num);
    }
  }


  void TestMain::doTest(int kind, int num) {
    const int tests = 10;
    double result[tests];
    double vmin = 0.0, vmax = 0.0;
    int nmin = 0, nmax = 0;
    assert(num > 0);

    if (m_allResult.size() < static_cast<size_t>(num)) m_allResult.resize(num);
    m_allResult[num-1].resize(tests+1);

    if (m_testBody == 0 || m_testBody->size() <= static_cast<size_t>(kind)) {
      outputMessage(str(format("unknown test kind %d" ENDL ) % kind));
      return;
    }

    ITestBody::ptr testBody = (*m_testBody)[kind];
    m_latestKind = kind;

    if (num > testBody->getTestCount()) {
      outputMessage("no test number" ENDL );
      return;
    }
    outputMessage(str(format("TEST:'%s'\r\nTestNumber#%d (%s) start!" ENDL ) 
                      % testBody->getTestName() 
                      % num 
                      % testBody->getSubTitle(num)));

    for (int n = 0; n < tests; ++n) {
      double past = 0.0;
      past = (*m_testBody)[kind]->doTest(num, n+1, m_testForm);
      outputMessage(str(format("<try%2d>  %.6lf msec past" ENDL ) % (n + 1) % (1000.0*past)));
      m_allResult[num-1][n] = past;
      result[n] = past;
      if (n == 0) {
        vmin = past; 
        vmax = past;
      }
      else {
        if (past < vmin) { vmin = past; nmin = n; }
        if (past > vmax) { vmax = past; nmax = n; }
      }
    }

    // 集計
    double sum = 0.0;
    for (int n = 0; n < tests; ++n) {
      if (n != nmin && n != nmax) {
        sum += result[n];
      }
    };
    // 結果の登録
    double average = sum / (tests-2);
    m_allResult[num-1][tests] = average;
    outputMessage(str(format("TestNumber#%d (%s) finish! average time %.6lf ms : %u loops per test : unit time %.6lf us" ENDL  ENDL ) 
                            % num 
                            % testBody->getSubTitle(num) 
                            % (1000.0*average) 
                            % testBody->getTestLoops(num) 
                            % (1000000.0*average / testBody->getTestLoops(num)) ));
    if (num == 1) {
      m_result1 = average; // 基準となる値
    }
    if (m_result1 != 0.0) {
      double pc = average / m_result1 * 100.0;
      m_testForm->setResultMsg(num, str(format("%.6lfms/%.2f%%") % (1000.0*average) % pc));
    }
  }

  void TestMain::saveResult(const std::string& fn) {
    if (m_allResult.empty()) {
      outputMessage("save result faild.");
      return;
    }
    ITestBody::ptr testBody = (*m_testBody)[m_latestKind];
    if (!testBody) {
      outputMessage("save result: invalid latest test.:" + fn);
      return;
    }

    ofstream ofs(fn.c_str(), ios::out | ios::app);
    if (!ofs.is_open()) {
      outputMessage("save result: file open faild.:" + fn);
      return;
    }


    int num = 0;
    double firstAverage = 0.0;
    BOOST_FOREACH(vector<double>& col, m_allResult) {
      if (num == 0) {
        ofs << testBody->getTestName() << ",";
        for (int n = 0; n < static_cast<int>(col.size()-1); ++n) {
          ofs << "try:" << (n+1) << ",";
        }
        ofs << "average, %" << endl;
      }
      ++num;
      ofs << "Test#" << num << ": " << testBody->getSubTitle(num) << ",";
      BOOST_FOREACH(double data, col) {
        ofs << data << ",";
      }
      if (num == 1) {
        ofs << "100.000" << endl;
        firstAverage = col[col.size()-1];
      }
      else {
        ofs << col[col.size()-1] / firstAverage * 100.0 << endl;
      }
    }
    outputMessage("save result succeeded.:" + fn);
  }

  // 各測定用のループより呼ばれるダミーの関数
  // 最適化によってループが除外されることを防ぎます
  volatile int TestMain::m_tick = 0;
  #pragma optimize( "", off )
  volatile void TestMain::tick(int sum) {
    m_tick += sum;
  }
  volatile void TestMain::tick(void* ptr) {
    m_tick += ptr != 0;
  }

  #pragma optimize( "", on )



}


