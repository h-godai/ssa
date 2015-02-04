#include "stdafx.h"
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "TestMain.h"
#include "libTS/Common/Common.h"
#include "libTS/math/piml/piml.hpp"
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"
#include <math.h>

using namespace std;
//using namespace boost;
using namespace boost::io;

namespace {


}


namespace app {

  using ts::MeasureTime;


  class PimlTest : public ITestBody {
  public:
    virtual ~PimlTest() {}

    virtual const char* getTestName() const { return "100 PIML performance test"; }

    virtual const char* getSubTitle(int num) {
      switch(num) {
        case 1: return "sqrt performance (float libc)";
        case 2: return "sqrt performance (float piml)";
        case 3: return "sqrt performance (float piml normal)";
        case 4: return "sin performance (float piml libc)";
        case 5: return "sin performance (float piml normal)";
        default: return "no test";
      }
    };

    // テストプログラムの個数を返す
    virtual int getTestCount() const { return 5; }

    // テストの実行
    virtual double doTestImpl(int testNum, int sequence) {
      switch (testNum) {
      case 1: return test1(sequence);
      case 2: return test2(sequence);
      case 3: return test3(sequence);
      case 4: return test4(sequence);
      case 5: return test5(sequence);
      default:
        return 0.0;
      }
    }
    static const int LoopCount = 10000000;
    // 1テストあたりのループ数を返す
    virtual size_t getTestLoops(int) const { return LoopCount; }


    double test1(int times) {
      if (times == 1) {
        outputMessage("sqrt performance for standard library" ENDL);
      }
      MeasureTime time;
      time.start();
      float a = 0.0001f;
      for (int i = 0; i < LoopCount; ++i) {
        a += ::sqrtf((float)(i%256));
      }
      time.stop();
      TestMain::tick(&a);
      return time.getTotalTime();
    }

    double test2(int times) {
      if (times == 1) {
        outputMessage("sqrt performance for piml sqrt" ENDL);
      }
      MeasureTime time;
      time.start();
      float a = 0.0001f;
      for (int i = 0; i < LoopCount; ++i) {
        a += piml::sqrt((float)(i%256));
      }
      time.stop();
      TestMain::tick(&a);
      return time.getTotalTime();
    }
    double test3(int times) {
      if (times == 1) {
        outputMessage("sqrt performance for piml sqrt normal" ENDL);
      }
      MeasureTime time;
      time.start();
      float a = 0.0001f;
      for (int i = 0; i < LoopCount; ++i) {
        a += piml::sqrt_normal((float)(i%256));
      }
      time.stop();
      TestMain::tick(&a);
      return time.getTotalTime();
    }
    double test4(int times) {
      if (times == 1) {
        outputMessage("sin performance for libc sinf" ENDL);
      }
      MeasureTime time;
      time.start();
      float a = 0.0001f;
      for (int i = 0; i < LoopCount; ++i) {
        a += ::sinf((float)(i%8));
      }
      time.stop();
      TestMain::tick(&a);
      return time.getTotalTime();
    }
    double test5(int times) {
      if (times == 1) {
        outputMessage("sin performance for piml sin 16-10" ENDL);
      }
      MeasureTime time;
      time.start();
      float a = 0.0001f;
      for (int i = 0; i < LoopCount; ++i) {
        a += piml::sin_cos_table<14>::sin(piml::theta<16>(piml::radian((float)(i%8))));
      }
      time.stop();
      TestMain::tick(&a);
      return time.getTotalTime();
    }
  };

  struct PimlTestInitializer {
    PimlTestInitializer() {
      TestMain::addTestBody(shared_ptr<ITestBody>(new PimlTest()));
    }
  } PimlTestInitializer;

}

