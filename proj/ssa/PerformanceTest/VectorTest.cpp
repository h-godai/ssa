#include "stdafx.h"
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include "TestMain.h"
#include "VectorTest.h"
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"

using namespace std;
using namespace boost;
using namespace boost::io;

//#define TICK(x) (x = x + x/2)
//#define TICK(x)
//#define TICK(x) ::srand(x)
#define TICK(x) TestMain::tick(x)

namespace app {
#ifdef _DEBUG
  const size_t LOOPS = 500000;
#else
  const size_t LOOPS = 50000000;
#endif

  using ts::MeasureTime;


  struct VectorTestInitializer {
    VectorTestInitializer() {
      TestMain::addTestBody(shared_ptr<ITestBody>(new VectorTest()));
    }
  } vectorTestInitializer;
 
  double VectorTest::test1(int /*times*/) {
    MeasureTime time;
    vector<int> vec(LOOPS);
    int sum = 0;
    vector<int>::iterator end = vec.end();
    time.start();
    for (vector<int>::iterator i = vec.begin(); i != end; ++i) {
      TICK(*i);
      sum += *i;
    }
    time.stop();
    TestMain::tick(sum); // 最適化でsumが消滅しないように
    return time.getTotalTime();
  }

 double VectorTest::test4(int /*times*/) {
    MeasureTime time;
    vector<int> vec(LOOPS);
    int sum = 0;
    time.start();
    BOOST_FOREACH(int& val, vec) {
      TICK(val);
      sum += val;
    }
    time.stop();
    TestMain::tick(sum); // 最適化でsumが消滅しないように
    return time.getTotalTime();
  }


    struct Sum {
      int& sum_;
      Sum(int& sum) : sum_(sum) {}
      void operator()(int val) {
        TICK(val);
        sum_ += val;
      }
    };

  double VectorTest::test2(int /*times*/) {
    MeasureTime time;
    vector<int> vec(LOOPS);
    int sum = 0;

	Sum sumobj(sum);

    time.start();
    for_each(vec.begin(), vec.end(), sumobj); //Sum(sum));
    time.stop();
    TestMain::tick(sum); // 最適化でsumが消滅しないように
    return time.getTotalTime();
  }

  double VectorTest::test3(int /*times*/) {
    MeasureTime time;
    int* vec = new int[LOOPS];
    int* v = vec;
    int sum = 0;
    time.start();
    for (uint32_t n = 0; n < LOOPS; ++n, ++v) {
     TICK(*v);
      sum += *v;
    }
    time.stop();
    TestMain::tick(sum); // 最適化でsumが消滅しないように
    delete [] vec;
    return time.getTotalTime();
  }
 
}

