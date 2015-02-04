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
#include <vector>
#include <string>
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace boost;
using namespace std;

#include <ITestBody.h>
#include "TestMain.h"

#define MAKE_LOOP_10(x) x;x;x;x;x;x;x;x;x;x;

#include "CppAdventForC.h"
#include <boost/range/algorithm/for_each.hpp>

namespace {
  using boost::uint16_t;

  template <int PixelBits>
  struct pixel_t {};
  template <>
  struct pixel_t<16> {
    uint16_t value_;
    pixel_t() : value_(0) {}
    pixel_t(int r, int g, int b)
        : value_(static_cast<uint16_t>(((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3)))
    {}
    int get_r() const { return (value_ >> 8) & 0xf8; }
    int get_g() const { return (value_ >> 3) & 0xfc; }
    int get_b() const { return (value_ << 3) & 0xff; }
  };
  typedef vector<pixel_t<16> > frame_buffer_t;
  struct transcolor {
      template <typename Pixel>
      void operator () (Pixel& px) {
          px = Pixel(px.get_g(), px.get_b(), px.get_r());
      }
  };

}

namespace app {

  using std::vector;
  using std::string;
  using ts::MeasureTime;

  class CppAdventTest2 : public ITestBody {
    static const size_t LOOPS = 100000;
    static const size_t LOOPS2 = 1000;
    static const size_t LOOPS3 = IMAGE_WIDTH*IMAGE_HEIGHT;
  public:
	CppAdventTest2()
	{

	}
    virtual ~CppAdventTest2() {}

    virtual const char* getTestName() const { return "011 C++ Advent 2012 part2"; }
    virtual size_t getTestLoops(int testnum) const {
      if (testnum < 4)
    	return LOOPS3; 
      else
        return LOOPS2;
    }

    virtual const char* getSubTitle(int num) {
      switch(num) {
        case 1: return "bitmap color transform by C";
        case 2: return "bitmap color transform by C macro";
        case 3: return "bitmap color transform by C++";

        default: return "no test";
      }
    };

    // テストプログラムの個数を返す
    virtual int getTestCount() const { return 3; }

    // テストの実行
    virtual double doTestImpl(int testNum, int sequence) {
      switch (testNum) {
      case 1: return test_bitmap_color_transform_c(sequence);
      case 2: return test_bitmap_color_transform_c_macro(sequence);
      case 3: return test_bitmap_color_transform_cpp(sequence);
      default:
        return 0.0;
      }
    }
  private:
    // RGB 5:6:5 の16bit bitmap  rrrr rggg gggb bbbb
    double test_bitmap_color_transform_c(int times) {
      MeasureTime time;
	  typedef uint16_t pixel_t;
	  pixel_t* buffer = new pixel_t[IMAGE_WIDTH * IMAGE_HEIGHT];
	  time.start();
	  test_bitmap_color_transform_pure_c(buffer);
	  time.stop();
	  delete [] buffer;
	  return time.getTotalTime();
    }
    double test_bitmap_color_transform_c_macro(int times) {
      MeasureTime time;
        typedef uint16_t pixel_t;
        pixel_t* buffer = new pixel_t[IMAGE_WIDTH * IMAGE_HEIGHT];
        time.start();
        test_bitmap_color_transform_macro_c(buffer);
        time.stop();
        delete [] buffer;
        return time.getTotalTime();
    }

    double test_bitmap_color_transform_cpp(int times) {
        MeasureTime time;
        frame_buffer_t buffer(IMAGE_WIDTH * IMAGE_HEIGHT);
        time.start();
		bitmap_color_transform_cpp(buffer);
        time.stop();
        return time.getTotalTime();
    }

  };

	struct CppAdvent2TestInitializer {
  	CppAdvent2TestInitializer() {
    	TestMain::addTestBody(shared_ptr<ITestBody>(new CppAdventTest2()));
	  }
	} cppAdvent2TestInitializer;



} // namespace app;

