// -*-tab-width:4-*-
#include "trigonometric.hpp"
#include "sqrt.hpp"
#include <math.h>
#include <fstream>
#include <iomanip>

namespace piml {



  float atan2(float x, float y) {
    float r = piml::sqrt(x*x+y*y); // 正規化
    if (r == 0.0f) return 0.0f;
    if (y == 0.0f) return PIf/2.0f;
    float xx = x / r;
//    return ::asinf(xx);
    return asin_table<11>::arc_sin(xx);
  }

  template <int BITS>
  float asin_table<BITS>::arc_cos(float x) {
    return arc_sin(piml::sqrt(1.0f - x*x));
  }





  // arctan を真面目に計算する。
  // Taylor展開(Maclaurin展開) 
  float arctan(float xx) {
    static const float ktbl[] = { 1.f/3.f
                                , 1.f/5.f
                                , 1.f/7.f
                                , 1.f/9.f
                                , 1.f/11.f
                                , 1.f/13.f
    };
    static const size_t ktbl_size = sizeof(ktbl) / sizeof(float);

    if (xx < 0.6f) {  // 0.6以下はテイラー展開 arctan(1/n)
      float x = 1.0f/xx;
      float x2 = x * x;
      float b = x / x2;
      float a = b;
      for (int k = 0 ; k < ktbl_size ;) {
        b /= x2;
        a -= b * ktbl[k];
        b /= x2;
        a += b * ktbl[++k];
        ++k;
      }
      return a;
    }
    else if (xx < 2.0f) { // 0.6-2.0は別の公式(近似式)
      // atan(1+1/x) = pi/4 + 1/2/x - 1/4/x^2 + 1/12/x^3 - 0 -1/40/x^5 + 1/48/x^6 
      float x = 1.0f/(xx-1.0f); //(1+1/x)の解
      float a = PIf/4.0f + 0.5f/x; // 1.f/2.f/x;
      float x2 = x * x;
      float b = x2;
      a -= 1.f/4.f/b;  b *= x; // b = x^3
      a += 1.f/12.f/b; b *= x2; // b = x^5
      a -= 1.f/40.f/b;  b *= x;  // b = x^6
      a += 1.f/48.f/b;  b *= x;  // b = x^7
      a -= 1.f/112.f/b; b *= x2; // b = x^9
      a += 1.f/288.f/b;
      return a;
    }
    else if (xx < 5.0f) { // 2.0-5.0は線形補完
      const float atan2 = 1.107149f;
      const float atan25 = 1.190290f;
      const float atan3 = 1.249046f;
      const float atan4 = 1.325818f;
      const float atan5 = 1.373401f;
      if (xx < 2.5f)
        return atan2 + (xx - 2.0f) * (atan25 - atan2) * 2.f;
      if (xx < 3.0f)
        return atan25 + (xx - 2.5f) * (atan3 - atan25) * 2.f;
      if (xx < 4.0f)
        return atan3 + (xx - 3.0f) * (atan4 - atan3);
      else
        return atan4 + (xx - 4.0f) * (atan5 - atan4);
    }
    else { // 5以上はテイラー展開 (通常版)
      // atan(x) = pi/2 - 1/x + 1/3/x^3 - 1/5/x^5 + 1/7/x^7 - . . . .
      float x = xx;
      float a = PIf/2.0f - (1.f/x);
      float b = x;
      float x2 = x * x;
      if (x > 10.0f) return a;
      for (int k = 0 ; k < ktbl_size ; ++k) {
        b *= x2; // x^3 , x^5, ...
        a -= ktbl[k] / b;
        b *= x2; // x^7 , x^9, ...
        a += ktbl[++k] / b;
      }
      return a;
    }
  }

  template <int BITS>
  template <typename OutStream>
  void sin_cos_table<BITS>::make_table(OutStream& os) {
    os << "template <> const float sin_cos_table<" << BITS << ">::sinTable_[] = {\n\t";
    for (int th = 0; th < sin_cos_table<BITS>::TableSize; ++th) {
        float v = ::sinf(th *3.14159265f*2.0f/Resolution);
        os << std::fixed << v << "f, ";
        if (((th+1) % 16) == 0) os << "\n\t";
    }
    os << "};\n";
  }

  template <int BITS>
  template <typename OutStream>
  void asin_table<BITS>::make_table(OutStream& os) {
     //os.setf(std::ios_base::scientific, std::ios_base::floatfield);
     os.setf(std::ios_base::fixed,std::ios_base::floatfield);
    os << "template <> const float asin_table<" << BITS << ">::asinTable_[] = {\n\t";
    for (int th = 0; th < asin_table<BITS>::Resolution; ++th) {
        float v = ::asinf(1.0f * th /  asin_table<BITS>::Resolution);
        os << std::setprecision(9) << v << "f, ";
        if (((th+1) % 16) == 0) os << "\n\t";
    }
    os << "};\n";
  }

  void make_sin_table10(const char* fn) {
    std::ofstream os(fn);
    os << "// ts trigonometric sin cos talbe\n";
    os << "#include \"trigonometric.hpp\"\n";
    os << "namespace piml {\n";
    sin_cos_table<10>::make_table(os);
    sin_cos_table<11>::make_table(os);
    asin_table<11>::make_table(os);
    asin_table<12>::make_table(os);
    os << "} // namespace piml\n\n";
  }
}

