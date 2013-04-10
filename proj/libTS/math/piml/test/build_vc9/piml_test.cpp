// piml_test.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "math/piml/piml.hpp"
#include "Util/unittest.hpp"
#include "Util/Logger.h"
#include <math.h>
#include <string>

namespace piml {
  using namespace endian;
  extern void make_sin_table10(const char* fn);
  using std::string;

  struct piml_test {
    POINTCUT_UNITTEST_METHOD;
  };

  template <typename Float> 
  void sqrt_test(ts::Unittest* test, const char* name) {
    test->message(name);

    Float sctho = 1.0f / 1000.0f; // 0.1%以上の誤差が出たらエラー
    Float maxdiff = 0.0f;
    Float maxc = 0.0f;
    bool ok = true;
    for (Float c = 0.000001f; c < 0.001f; c += 0.000001f) {
      Float diff = fabs(1.0f - (piml::sqrt(c) / ::sqrt(c)));
      if (diff > maxdiff) { maxdiff = diff; maxc = c; }
      if (diff > sctho) { ok = false; TS_DEBUGLOG("sqrt(%f) diff:%f%%  piml(%f) as libc(%f)\n", c, diff * 100.0f, piml::sqrt(c), ::sqrt(c)); }
    }
    test->check(ok, "0.000001 - 0.001 step:0.000001");
    ok = true;
    for (Float c = 0.001f; c < 10.0f; c += 0.001f) {
      Float diff = fabs(1.0f - (piml::sqrt(c) / ::sqrt(c)));
      if (diff > maxdiff) { maxdiff = diff; maxc = c; }
      if (diff > sctho) { ok = false; TS_DEBUGLOG("sqrt(%f) diff:%f%%  piml(%f) as libc(%f)\n", c, diff * 100.0f, piml::sqrt(c), ::sqrt(c)); }
    }
    test->check(ok, "0.001 - 10.0 step:0.001");
    ok = true;
    for (Float c = 10.0f; c < 1000.0f; c += 0.1f) {
      Float diff = fabs(1.0f - (piml::sqrt(c) / ::sqrt(c)));
      if (diff > maxdiff) { maxdiff = diff; maxc = c; }
      if (diff > sctho) { ok = false; TS_DEBUGLOG("sqrt(%f) diff:%f%%  piml(%f) as libc(%f)\n", c, diff * 100.0f, piml::sqrt(c), ::sqrt(c)); }
    }
    test->check(ok, "10.0 - 1000.0 step:0.1");
    ok = true;
    for (Float c = 1000.0f; c < 100000.0f; c += 1.0f) {
      Float diff = fabs(1.0f - (piml::sqrt(c) / ::sqrt(c)));
      if (diff > maxdiff) { maxdiff = diff; maxc = c; }
      if (diff > sctho) { ok = false; TS_DEBUGLOG("sqrt(%f) diff:%f%%  piml(%f) as libc(%f)\n", c, diff * 100.0f, piml::sqrt(c), ::sqrt(c)); }
    }
    test->check(ok, "1000.0 - 100000.0 step:1.0");
    ok = true;
    for (Float c = 100000.0f; c < 10000000.0f; c += 100.0f) {
      Float diff = fabs(1.0f - (piml::sqrt(c) / ::sqrt(c)));
      if (diff > maxdiff) { maxdiff = diff; maxc = c; }
      if (diff > sctho) { ok = false; TS_DEBUGLOG("sqrt(%f) diff:%f%%  piml(%f) as libc(%f)\n", c, diff * 100.0f, piml::sqrt(c), ::sqrt(c)); }
    }
    test->check(ok, "100000.0 - 10000000.0 step:100.0");
    ok = true;
    TS_DEBUGLOG("*** sqrt result max-diff:%f%% max-c:%f\n", maxdiff * 100.0f, maxc);
  }

  template <size_t THBITS, size_t SINBITS>
  void trigonometric_test(ts::Unittest* test, const char* msg) {
    test->message(msg);
    typedef theta<THBITS> Th;
    typedef sin_cos_table<SINBITS> SinCos;

    float sdiffave = 0.0f;
    float sdiffmax = 0.0f;
    float cdiffave = 0.0f;
    float cdiffmax = 0.0f;
    int cnt = 0;
    for (float rad = -2.0f*PIf; rad < 2.0f*PIf; rad += 0.01f) {
      Th th = radian(rad);
      float s = SinCos::sin(th);
      float c = SinCos::cos(th);
      float ss = ::sinf(rad);
      float cc = ::cosf(rad);
      float sdiff = fabs(s - ss);
      float cdiff = fabs(c - cc);
      if (sdiff > sdiffmax) sdiffmax = sdiff;
      sdiffave += sdiff;
      if (cdiff > cdiffmax) cdiffmax = cdiff;
      cdiffave += cdiff;
      ++cnt;
    }
    test->check(sdiffmax < 0.01f, "sin diff check");
    test->check(cdiffmax < 0.01f, "cos diff check");
    TS_DEBUGLOG("sin count:%d diff-max:%f diff-ave:%f\n", cnt, sdiffmax, sdiffave/cnt);
    TS_DEBUGLOG("cos count:%d diff-max:%f diff-ave:%f\n", cnt, cdiffmax, cdiffave/cnt);

    float tdiffave = 0.0f;
    float tdiffmax = 0.0f;
    cnt = 0;
    for (float rad = -0.40f*PIf; rad < 0.40f*PIf; rad += 0.01f) {
      Th th = radian(rad);
      float t = SinCos::tan(th);
      float tt = ::tanf(rad);
      //TS_DEBUGLOG("tan(%f) piml:%f std:%f\n", rad, t, tt);
      if (t != 0.0f && tt != 0.0f) {
        float tdiff = fabs(1.0f - (t / tt));
        if (tdiff > tdiffmax) tdiffmax = tdiff;
        tdiffave += tdiff;
        ++cnt;
      }
    }
    test->check(tdiffmax < 1.0f, "tan diff check");
    TS_DEBUGLOG("tan count:%d diff-max:%f diff-ave:%f\n", cnt, tdiffmax, tdiffave/cnt);

  }

  void atan_test(ts::Unittest* test) {
    float diffmax = 0.0f;
    float maxx = 0.0f;
    for (float x = 0.1f; x < 10.0f; x += 0.1f) {
      float diff = fabs(1.0f - (::atanf(x)/arctan(x))) * 100.f;
      if (diffmax < diff) { maxx = x; diffmax = diff; }
      if (diff > 0.01f) {
        TS_DEBUGLOG("atan(%f) piml:%f  std:%f diff:%f%% max=%f%%\n", x, arctan(x), ::atanf(x), diff, diffmax);
      }
    }
    for (float x = 10.f; x < 100.0f; x += 1.0f) {
      float diff = fabs(1.0f - (::atanf(x)/arctan(x))) * 100.f;
      if (diffmax < diff) {maxx = x; diffmax = diff;}
      if (diff > 0.01f) {
        TS_DEBUGLOG("atan(%f) piml:%f  std:%f diff:%f%% max=%f%%\n", x, arctan(x), ::atanf(x), diff, diffmax);
      }
    }
    for (float x = 100.f; x < 10000.0f; x += 100.0f) {
      float diff = fabs(1.0f - (::atanf(x)/arctan(x))) * 100.f;
      if (diffmax < diff) {maxx = x; diffmax = diff;}
      if (diff > 0.01f) {
        TS_DEBUGLOG("atan(%f) piml:%f  std:%f diff:%f%% max=%f%%\n", x, arctan(x), ::atanf(x), diff, diffmax);
      }
    }
    test->check(diffmax < 1.0f, "piml atan test");
    TS_DEBUGLOG("atan max diff %f%% x = %f\n", diffmax, maxx);
  }

  bool piml_test::unittest(ts::Unittest* test) {

    float maxdiff = 0.0f;
    int cnt = 0;
    for (float x = 5.0f; x < 100.0f; x += 1.f) {
      for (float y = 5.0f; y < 100.0f; y += 1.f) {
        float p = piml::atan2(x,y);
        float s = ::atan2(x,y);
        float diff = s == 0.0f ? p : fabs(1.0f - (p/s)) * 100.0f;
        if (diff > maxdiff) maxdiff = diff;
        if (diff > 0.1f) {
          TS_DEBUGLOG("atan2(%f,%f)  piml::%f std::%f diff:%f%%\n", x,y, piml::atan2(x,y), ::atan2(x,y), diff);
          ++cnt;
        }
      }
    }
    TS_DEBUGLOG("atan2 max diff=%f (%d)\n", maxdiff, cnt);



    atan_test(test);

    make_sin_table10("trigonometric_table.cpp");

    trigonometric_test<16,10>(test, "trigonometric_test theta 16bit sin/cos 10bit\n");


    sqrt_test<float>(test,  "* sqrt test for float  \n");
    sqrt_test<double>(test, "* sqrt test for double \n");

    uint16_t ui16 = 0x1234U;
    test->check(bswap(ui16) == 0x3412, "bswap 16");
 
    uint32_t ui32 = 0x12345678U;
    test->check(bswap(ui32) == 0x78563412, "bswap 32");

    uint64_t ui64 = 0x123456789abcdef0ULL;
    test->check(bswap(ui64) == 0xf0debc9a78563412ULL, "bswap 64");

    enum e {
      e12345678 = 0x12345678,
      e78563412 = 0x78563412
    };
    test->check(bswap(e12345678) == e78563412, "bswap enum");

    float fl32 = 0x801234;  // 0x4b001234 0100 1011 0000 0000 0001 0010 0011 0100
                            // 0x3412004b 0011 0100 0001 0010 0000 0000 0100 1011
    float fl32s = bswap(fl32);
    test->check((*(unsigned*)(&fl32s)) == 0x3412004b, "bswap float");
    test->check(fl32 == bswap(fl32s), "bswap float2");

    union {
      double dbl;
      uint8_t u[8];
    } d1, d2;
    d1.dbl = 3.14159265358979;
    d2.dbl = bswap(d1.dbl);
    int ok = 0;
    for (int i = 0; i < 8; ++i) {
      if (d1.u[i] == d2.u[7-i]) ++ok;
    }
    test->check(ok == 8, "bswap double");
    test->check(d1.dbl == bswap(d2.dbl), "bswap double2");
    return true;
  }



  POINTCUT_UNITTEST_REGIST(piml_test);
}


int main(int /*argc*/, char* /*argv*/[])
{
  ts::Unittest::Instance().run("PIML test");
	return 0;
}

