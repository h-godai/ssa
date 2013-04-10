/** size.hpp -*-tab-width:4;mode:C++-*-





*/


#ifndef TS_MATH_SIZE_HPP
#define TS_MATH_SIZE_HPP

#include <utility>
#include "arithmetic_operations.hpp"

namespace ts {
namespace math {


  template <typename T>
  struct size_T : arithmetic_operations2<size_T<T>, T> {
	union {
	  T x_;
	  T width_;
	};
	union {
	  T y_;
	  T height_;
	};
	size_T() : width_(0), height_(0) {}
	template <typename TT>
	size_T(TT w, TT h) : width_(static_cast<T>(w)), height_(static_cast<T>(h)) {}
	template <typename TT>
	size_T(const size_T<TT>& s) : width_(static_cast<T>(s.width_)), height_(static_cast<T>(s.height_)) {}

	T x() const { return x_; }
	T y() const { return y_; }


	bool operator == (const size_T& dst) const {
	  return width_ == dst.width_ && height_ == dst.height_;
	}
      size_T<T>& operator = (const size_T& src) {
          width_ = src.width_;
          height_ = src.height_;
          return *this;
      }

	void swap(size_T& sz) throw() {
	  std::swap(width_, sz.width_);
	  std::swap(height_, sz.height_);
	}
	bool empty() const { return width_ == 0 && height_ == 0; }
  };

  typedef size_T<double> size_d;
  typedef size_T<float> size_f;
  typedef size_T<int32_t> size_i;
  typedef size_T<uint32_t> size_u;


}}
#endif

