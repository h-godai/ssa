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
#include "SortTest.h"
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"

using namespace std;
using namespace boost;
using namespace boost::io;

namespace {

  struct Element {
    double x,y,z;
    bool operator () (const Element& a, const Element& b) const {
      return a.x+a.y+a.z < b.x+b.y+b.z;
    }
  };

  int compare(const void* va, const void* vb) {
    const Element& a = *static_cast<const Element*>(va);
    const Element& b = *static_cast<const Element*>(vb);
    if (a.x+a.y+a.z < b.x+b.y+b.z) return 1;
    if (a.x+a.y+a.z > b.x+b.y+b.z) return -1;
    return 0;
  }

  static vector<Element> element;

}


namespace app {

  using ts::MeasureTime;


  struct SortTestInitializer {
    SortTestInitializer() {
      TestMain::addTestBody(shared_ptr<ITestBody>(new SortTest()));
    }
  } SortTestInitializer;

  SortTest::SortTest() {  }

  void SortTest::createSrc(int seed) {
    srand(seed);
    element.resize(ELEMENT_SIZE);
    BOOST_FOREACH(Element& e, element) {
      e.x = rand() / (1 + rand());
      e.y = rand() / (1 + rand());
      e.z = rand() / (1 + rand());
    }
  }



  double SortTest::test1(int times) {
    if (times == 1) {
      outputMessage(str(format("%d float64 elements\r\n") % ELEMENT_SIZE));
    }
    createSrc(times);
    MeasureTime time;
    time.start();
    qsort(static_cast<void*>(&element[0]), ELEMENT_SIZE, sizeof(Element), compare);
    time.stop();
    return time.getTotalTime();
  }

  double SortTest::test2(int times) {
    if (times == 1) {
      outputMessage(str(format("%d float64 elements\r\n") % ELEMENT_SIZE));
    }
    createSrc(times);
    MeasureTime time;
    time.start();
    sort(element.begin(), element.end(), Element());
    time.stop();
    return time.getTotalTime();
  }

}

