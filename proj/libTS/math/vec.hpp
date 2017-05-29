/** vec.hpp -*-tab-width:4;mode:C++-*-





*/


#ifndef TS_MATH_VEC_HPP
#define TS_MATH_VEC_HPP

#include "arithmetic_operations.hpp"
#include "pos.hpp"

namespace ts {
namespace math {
  
  template <typename T>
  struct vec2 : arithmetic_operations2<vec2<T>, T> {
	typedef T value_type;
	T x_;
	T y_;
	vec2() : x_(0), y_(0) {}
	vec2(const vec2& v) : x_(v.x_), y_(v.y_) {}
	template <typename TT>
	vec2(TT x, TT y) : x_(static_cast<T>(x)), y_(static_cast<T>(y)) {}
	template <typename TT>
	vec2(const TT& v) : x_(static_cast<T>(v.x_)), y_(static_cast<T>(v.y_)) {}
	template <typename TT>
	vec2(const pos2<TT>& v1, const pos2<TT>& v2) : x_(static_cast<T>(v2.x_-v1.x_)), y_(static_cast<T>(v2.y_-v1.y_)) {}

	T x() const { return x_; }
	T y() const { return y_; }

	operator bool () const {
	  return !(x_ == 0 && y_ == 0);
	}
	
  	T length() const {
	  return static_cast<T>(sqrt(x_*x_ + y_*y_));
	}
	void set_length(T len) {
      T w = len / sqrt(x_*x_ + y_*y_);
	  x_ *= w;
	  y_ *= w;
	}

    vec2& normalize() {
      T w = static_cast<T>(1.0) / sqrt(x_*x_ + y_*y_);
      x_ *= w;
      y_ *= w;
      return *this;
    }

    // vとの外積を求める!
    T cross(const vec2& v) const {
      return (x_ * v.y_) - (v.x_ * y_);
    }

    // vとの内積を求める!
    T dot(const vec2& v) const {
      return x_ * v.x_ + y_ * v.y_;
    }



	void swap(vec2& s) { std::swap(x_, s.x_); std::swap(y_, s.y_); }

  };

  template <typename T>
  struct vec3 : arithmetic_operations3<vec3<T>, T> {
	typedef T value_type;
	T x_;
	T y_;
	T z_;
	vec3() : x_(0), y_(0), z_(0) {}
	vec3(const vec3& v) : x_(v.x_), y_(v.y_), z_(v.z_) {}
	template <typename TT>
	vec3(TT x, TT y, TT z) : x_(static_cast<T>(x)), y_(static_cast<T>(y)), z_(static_cast<T>(z)) {}
	template <typename TT>
	vec3(const TT& v) : x_(static_cast<T>(v.x_)), y_(static_cast<T>(v.y_)), z_(static_cast<T>(v.z_)) {}
	template <typename TT>
	vec3(const pos3<TT>& v1, const pos3<TT>& v2) : x_(static_cast<T>(v2.x_-v1.x_)), y_(static_cast<T>(v2.y_-v1.y_)), z_(static_cast<T>(v2.z_-v1.z_)) {}

	T x() const { return x_; }
	T y() const { return y_; }
	T z() const { return z_; }

	operator bool () const {
	  return !(x_ == 0 && y_ == 0 && z_ == 0);
	}
	
  	inline T length() const {
	  return sqrt(x_*x_ + y_*y_ + z_*z_);
	}
	void set_length(T len) {
      T w = len / length();
	  x_ *= w;
	  y_ *= w;
	  z_ *= w;
	}

    vec3& normalize() {
      T w = static_cast<T>(1.0) / length();
      x_ *= w;
      y_ *= w;
      z_ *= w;
      return *this;
    }
	void swap(vec3& s) { std::swap(x_, s.x_); std::swap(y_, s.y_); std::swap(z_, s.z_); }

    // vとの外積を求める!
    vec3 cross(const vec3& v) const {
      return vec3((y_*v.z_)-(z_*v.y_), (z_*v.x_)-(x_*v.z_), (x_*v.y_)-(y_*v.x_));
    }

    // vとの内積を求める!
    T dot(const vec3& v) const {
      return (x_ * v.x_) + (y_ * v.y_) + (z_ * v.z_);
    }


  };

  typedef vec2<double> vec2_d;
  typedef vec2<float> vec2_f;
  typedef vec2<int32_t> vec2_i;
  typedef vec3<double> vec3_d;
  typedef vec3<float> vec3_f;
  typedef vec3<int32_t> vec3_i;

}}
#endif

