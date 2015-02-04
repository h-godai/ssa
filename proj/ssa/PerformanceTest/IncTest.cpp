#include "stdafx.h"
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "TestMain.h"
#include "IncTest.h"
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"

using namespace std;
using namespace boost;
using namespace boost::io;

namespace {

  typedef map<int, int> ElementType;
  

  static ElementType mapElement;
  //bool initialized = false;
}


namespace app {
#ifdef _DEBUG
  const size_t INC_LOOP_SIZE = 500;
#else
  const size_t INC_LOOP_SIZE = 10000;
#endif

  using ts::MeasureTime;


  struct IncTestInitializer {
    IncTestInitializer() {
      TestMain::addTestBody(shared_ptr<ITestBody>(new IncTest1()));
      TestMain::addTestBody(shared_ptr<ITestBody>(new IncTest2()));
    }
  } IncTestInitializer;

  void IncTest1::createSrc(int seed) {
    srand(seed);
    mapElement.clear();
    for (unsigned int n = 0; n < INC_LOOP_SIZE; ++n) {
      mapElement[n] = rand();
    }
  }




  double IncTest2::test1(int times) {
    vector<int> element(INC_LOOP_SIZE);
    size_t count = element.size()-1;
    if (times == 1) {
      outputMessage(str(format("loop count: %d\r\n") % (count*INC_LOOP_SIZE)));
    }
    MeasureTime time;
    time.start();
    for (size_t n1 = 0; n1 < INC_LOOP_SIZE; n1++) {
      vector<int>::iterator i = element.begin()+1;
      for (size_t n2 = 0; n2 < count; n2++) {
        TestMain::tick(*i++);
      }
    }
    time.stop();
    return time.getTotalTime();
  }

  double IncTest2::test2(int times) {
    vector<int> element(INC_LOOP_SIZE);
    size_t count = element.size()-1;
    if (times == 1) {
      outputMessage(str(format("loop count: %d\r\n") % (count*INC_LOOP_SIZE)));
    }
    MeasureTime time;
    time.start();
    for (size_t n1 = 0; n1 < INC_LOOP_SIZE; ++n1) {
      vector<int>::iterator i = element.begin();
      for (size_t n2 = 0; n2 < count; ++n2) {
        TestMain::tick(* ++i);
      }
    }
    time.stop();
    return time.getTotalTime();
  }

  double IncTest1::test1(int times) {
    createSrc(times);
    size_t count = mapElement.size()-1;
    if (times == 1) {
      outputMessage(str(format("loop count: %d\r\n") % (count*INC_LOOP_SIZE)));
    }
    MeasureTime time;
    time.start();
    for (size_t n1 = 0; n1 < INC_LOOP_SIZE; n1++) {
      ElementType::iterator i = mapElement.begin(); i++;
      for (size_t n2 = 0; n2 < count; n2++) {
        TestMain::tick(i++ -> second);
      }
    }
    time.stop();
    return time.getTotalTime();
  }

  double IncTest1::test2(int times) {
    createSrc(times);
    size_t count = mapElement.size()-1;
    if (times == 1) {
      outputMessage(str(format("loop count: %d\r\n") % (count*INC_LOOP_SIZE)));
    }
    outputMessage(str(format("test count %d: ") % times));
    MeasureTime time;
    time.start();
    for (size_t n1 = 0; n1 < INC_LOOP_SIZE; ++n1) {
      ElementType::iterator i = mapElement.begin();
      for (size_t n2 = 0; n2 < count; ++n2) {
        TestMain::tick(++i -> second);
      }
      ++i;
    }
    time.stop();
    return time.getTotalTime();
  }

}

