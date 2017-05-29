/** math.hpp -*-tab-width:4;mode:C++-*-



 */
#include <assert.h>
#include <stdlib.h>

#ifndef TS_MATH_MATH_HPP
#define TS_MATH_MATH_HPP

namespace ts {


  template <typename T>
  int sign(const T& v) {
    return (v > 0) - (v < 0);
  }

  namespace math {
	const double pi = 3.1415925635897932384626433832795;
	const double div_pi = 1.0/3.1415925635897932384626433832795;
	const double k_rad2deg = 180.0 / pi;
	const double k_deg2rad = pi / 180.0;

	template <typename T>
	T radian2deg(const T& rad) { return rad * static_cast<T>(k_rad2deg); }
	template <typename T>
	T deg2radian(const T& dec) { return dec * static_cast<T>(k_deg2rad); }


	template <typename T>
	T random(T minval, T maxval) {
	  T range = maxval - minval;
	  return minval + (range * ::rand() / RAND_MAX);
	}
	template <>
	int random(int minval, int maxval) {
	  int range = maxval - minval;
	  return minval + (::rand() % range);
	}
	template <>
	unsigned int random(unsigned int minval, unsigned int maxval) {
	  assert(minval < maxval);
	  unsigned int range = maxval - minval;
	  return minval + (::rand() % range);
	}

  }


} // namespace ts;
#endif
