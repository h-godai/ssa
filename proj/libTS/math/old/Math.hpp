
#ifndef TS_MATH_HPP
#define TS_MATH_HPP
#pragma warning (push)
#pragma warning (disable : 4100)

#include <boost/concept_check.hpp>
#include <limits>

namespace ts { namespace math {

  const double PI = 3.141592653589793238462643383279;
  const double PI2 = (PI*2.0);



  // 高速な int log2
  template<typename IntT>
  inline int intlog2(IntT i) {
    boost::function_requires< boost::UnsignedIntegerConcept<IntT> >();
    int n = 1;
    int d = std::numeric_limits<IntT>::digits / 2;
    int s = 0;
    while (d > 1) {
      s += d;
      if ((i >> s) == 0) {
        n += d;
        i <<= d;
      }
      d /= 2;
    }
    n -= i >> (std::numeric_limits<IntT>::digits - 1);
    return std::numeric_limits<IntT>::digits - 1 - n;
  }


  // メタプログラミングのlog2
  template <int IntT>
  struct log2 {
	  static int const val = 1 + log2<(IntT >> 1)>::val;
  };

  template <>
  struct log2<1> {
	  static int const val = 0;
  };



} } // namespace ts::math

#pragma warning (pop)

#endif
