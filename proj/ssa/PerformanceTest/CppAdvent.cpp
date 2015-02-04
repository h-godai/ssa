// C++ Advent test -*-tab-width:4-*-
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdio.h>
#undef new
#undef delete
#undef malloc
#undef free

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace boost;
using namespace std;

#include <ITestBody.h>
#include "TestMain.h"

#define MAKE_LOOP_10(x) x;x;x;x;x;x;x;x;x;x;

#include <boost/range/algorithm/for_each.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <boost/unordered_map.hpp>

namespace {
   const size_t LOOPS3 = 100000;
}



namespace app {

  using std::vector;
  using std::string;
  using ts::MeasureTime;

  class CppAdventTest : public ITestBody {
    static const size_t LOOPS = 100000;
    static const size_t LOOPS2 = 1000;
    MeasureTime mt_;

    struct printpoolresult {
      CppAdventTest& self_;
      printpoolresult(CppAdventTest& s) : self_(s) {}
      void operator() (const std::pair<double, uint32_t>& ele) {
        self_.outputMessage(str(format("%.9lf    %d%s") % ele.first % ele.second % ENDL));
      }
    };


  public:
	CppAdventTest()
	{
      mt_.start();
	}
    virtual ~CppAdventTest() {}

    virtual const char* getTestName() const { return "010 C++ Advent 2012 "; }
    virtual size_t getTestLoops(int testnum) const {
      if (testnum < 3)
    	return LOOPS*10; 
      else if (testnum < 5)
	    return LOOPS2;
      else
        return LOOPS3;
    }

    virtual const char* getSubTitle(int num) {
      switch(num) {
        case 1: return "hello world by C";
        case 2: return "hello world by C++";
        case 3: return "text search by C";
        case 4: return "text search by C++";
        case 5: return "boost pool allocate time";
        case 6: return "boost pool allocate time";

        default: return "no test";
      }
    };

    // テストプログラムの個数を返す!
    virtual int getTestCount() const { return 6; }

    // テストの実行!
    virtual double doTestImpl(int testNum, int sequence) {
      switch (testNum) {
      case 1: return test_hello_world_c(sequence);
      case 2: return test_hello_world_cpp(sequence);
      case 3: return test_text_search_c(sequence);
      case 4: return test_text_search_cpp(sequence);
      case 5: return test_pool_time(sequence);
      case 6: return test_pool_time2(sequence);
      default:
        return 0.0;
      }
    }
  private:
    vector<double> pool_result_;
    map<double, uint32_t> pool_result_map_;
    uint64_t* ptrs_[LOOPS3];

    double test_pool_time(int times) {
      boost::object_pool<uint64_t> intpool;
      ofstream ofs("boost_pool_malloc_time.txt");
      return test_pool_time_t(times, intpool, ofs);
    }
    double test_pool_time2(int times) {
      boost::object_pool<uint64_t> intpool(LOOPS3);
      ofstream ofs("boost_pool_malloc_time_alloced.txt");
      return test_pool_time_t(times, intpool, ofs);
    }

    template <typename T>
    double test_pool_time_t(int times, T& intpool, ostream& ofs) {
      if (times == 1) {
        outputMessage("boost pool allocate time" ENDL);
        pool_result_.clear();
        pool_result_.resize(LOOPS3);
        pool_result_map_.clear();
      }
      MeasureTime time;
      // 計測開始!
      for (int n = 0; n < LOOPS3; ++n) {
        time.start();
        ptrs_[n] = intpool.malloc();
        *ptrs_[n] = time.stop();
      }
      // 結果集計!
      for (int n = 0; n < LOOPS3; ++n) {
          double t = time.toSecond(*ptrs_[n]);
          pool_result_[n] += t;
          ++pool_result_map_[t];
      }
      if (times == 10) {
        boost::for_each(pool_result_map_, printpoolresult(*this));
        for (int n = 0; n < LOOPS3; ++n) {
            // μ秒で出力!
          ofs << fixed << setprecision(3) <<  1000000.0 * pool_result_[n] / 10.0  << endl;

        }
      }

      return time.getTotalTime();
    }

    char* load_text() {
      FILE* f = fopen("textsample.txt", "r");
      if (f == 0) {
    	outputMessage("search text not found!" ENDL);
    	return 0;
      }
      struct stat st;
      fstat(fileno(f), &st);
      char* buf = new char[st.st_size+1];
      size_t sz = fread(buf, st.st_size, 1, f);
      buf[sz] = 0;
      return buf;
    }

    // search text;
    double test_text_search_c(int times) {

      MeasureTime time;
      const char* text = load_text();
      const char* searchtext = "BOOST";
      size_t tlen = strlen(searchtext);
      if (times == 1) {
        outputMessage(str(format("search text 'BOOST' in boost/foreach.hpp text size = %d%s") % strlen(text) % ENDL));
      }
      unsigned int found = 0;
      time.start();
      for (size_t loop = 0; loop < LOOPS2; ++loop) {
        const char* p = text;
        while (p && *p) {
          p = ::strstr(p, searchtext);
          if (p != 0) {
            ++found;
            p += tlen; // size of 'BOOST'
          }
        }
      }
      time.stop();

      if (text != 0) delete [] text;
      if (times == 1) {
	    outputMessage(str(format("%d BOOST found!" ENDL) % found));
      }
      return time.getTotalTime();
    }

    // search text
    double test_text_search_cpp(int times) {
      MeasureTime time;
      const char* txt = load_text();
      string text(txt);
      string search_text("BOOST");
      string::size_type tlen = search_text.length();
      unsigned int found = 0;
      time.start();
      for (size_t loop = 0; loop < LOOPS2; ++loop) {
	    string::size_type p = 0;
	    while (p != string::npos) {
	      p = text.find(search_text, p);
	      if (p != string::npos) {
	        ++found;
            p += tlen; // size of 'BOOST'
	      }
	    }
      }
      time.stop();
      if (times == 1) {
	outputMessage(str(format("%d BOOST found!" ENDL) % found));
      }
      if (txt != 0) delete [] txt;
      return time.getTotalTime();
    }
    double test_hello_world_c(int /*times*/) {
      MeasureTime time;
      

      time.start();
      for (size_t i = 0; i < LOOPS; ++i) {
        MAKE_LOOP_10(printf("Hello world\n"))
      }
      time.stop();
      return time.getTotalTime();
    }
    double test_hello_world_cpp(int /*times*/) {
      MeasureTime time;
      

      time.start();
      for (size_t i = 0; i < LOOPS; ++i) {
        MAKE_LOOP_10(std::cout << "Hello world" << std::endl);
      }
      time.stop();
      return time.getTotalTime();
    }
  };

	struct CppAdventTestInitializer {
  	CppAdventTestInitializer() {
    	TestMain::addTestBody(shared_ptr<ITestBody>(new CppAdventTest()));
	  }
	} cppAdventTestInitializer;



} // namespace app;

