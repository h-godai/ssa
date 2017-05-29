/* -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_COMMON_HPP
#define TS_APPGW_COMMON_HPP

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include <math.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <deque>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "libTS/Util/Logger.h"
#include "../exception.hpp"

namespace ts {
namespace appfw {
namespace gl {
  using boost::gil::rgba8_image_t;
  using boost::gil::rgb8_image_t;
  using boost::gil::view;
  using boost::shared_ptr;
  typedef boost::mutex::scoped_lock scoped_lock;

  struct glfw_exception : appfw_exception {
	glfw_exception(const char* const & msg) : appfw_exception(msg)
	{}
	virtual ~glfw_exception() throw() {}
  };
  
  template <typename C, typename VT>
  struct arithmetic_operations2 {
	typedef const C CC;
	C operator + (C p) const { CC& t = static_cast<CC&>(*this); p.x_ += t.x_; p.y_ += t.y_; return p; }
	C operator - (C p) const { CC& t = static_cast<CC&>(*this); p.x_ = t.x_ - p.x_; p.y_ = t.y_ - p.y_; return p; }
	C operator / (VT d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ / d, t.y_ / d); }
	C operator * (VT d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ * d, t.y_ * d); }
	C& operator /= (VT d) { C& t = static_cast<C&>(*this); t.x_ /= d; t.y_ /= d; return t; }
	C& operator *= (VT d) { C& t = static_cast<C&>(*this); t.x_ *= d; t.y_ *= d; return t; }
  };


  template <typename T>
  struct size : arithmetic_operations2<size<T>, T> {
	union {
	  T x_;
	  T width_;
	};
	union {
	  T y_;
	  T height_;
	};
	size() : width_(0), height_(0) {}
	template <typename TT>
	size(TT w, TT h) : width_(static_cast<T>(w)), height_(static_cast<T>(h)) {}
	bool operator == (const size& dst) const {
	  return width_ == dst.width_ && height_ == dst.height_;
	}
	void swap(size& sz) {
	  std::swap(width_, sz.width_);
	  std::swap(height_, sz.height_);
	}
  };

  template <typename T>
  struct pos2d : arithmetic_operations2<pos2d<T>, T>{
	typedef T value_type;
	T x_;
	T y_;
	pos2d() : x_(0), y_(0) {}
	pos2d(const pos2d& pos) : x_(pos.x_), y_(pos.y_) {}
	template <typename TT>
	pos2d(TT x, TT y) : x_(static_cast<T>(x)), y_(static_cast<T>(y)) {}
	template <typename TT>
	pos2d(const TT& pos) : x_(static_cast<T>(pos.x_)), y_(static_cast<T>(pos.y_)) {}


  };
  
  template <typename T>
  struct vec2 : arithmetic_operations2<vec2<T>, T>{
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
	vec2(const pos2d<TT>& v1, const pos2d<TT>& v2) : x_(static_cast<T>(v2.x_-v1.x_)), y_(static_cast<T>(v2.y_-v1.y_)) {}

	operator bool () const {
	  return !(x_ == 0 && y_ == 0);
	}

	void clear() { x_ = 0; y_ = 0; }

  	T length() const {
	  return sqrt(x_*x_ + y_*y_);
	}
	void set_length(T len) {
	  T l = length();
	  x_ = x_ * len / l;
	  y_ = y_ * len / l;
	}
  };


  

  template <typename T>
  struct rect {
	pos2d<T> pos_;
	size<T> size_;
	rect(const pos2d<T>& pos, const size<T>& size) 
	  : pos_(pos)
	  , size_(size)
	{}
	T left() const { return pos_.x_; }
	T top() const { return pos_.y_; }
	T right() const { return pos_.x_ + size_.width_; }
	T bottom() const { return pos_.y_ + size_.height_; }
	T width() const { return size_.width_; }
	T height() const { return size_.height_; }

	void set_size(size<T> size) {
	  size_.swap(size);
	}
	// pがrectの内側にあるか調べる
	template <typename PosT>
	bool is_inside(const PosT& p) const {
	  return p.x_ >= left() && p.x_ < right() && p.y_ >= top() && p.y_ < bottom();
	}
  };

  typedef pos2d<double> pos2d_d;
  typedef pos2d<float> pos2d_f;
  typedef pos2d<int32_t> pos2d_i;
  typedef size<double> size_d;
  typedef size<float> size_f;
  typedef size<uint32_t> size_i;
  typedef rect<float> rect_f;
  typedef rect<int32_t> rect_i;
  typedef vec2<double> vec2_d;
  typedef vec2<float> vec2_f;
  typedef vec2<int32_t> vec2_i;
  
  class context;

  struct cmd_base {
	enum cmd_type {
	  show_image_type
	};
	typedef shared_ptr<cmd_base> ptr;
	virtual bool execute(context& ) = 0;
	virtual cmd_type type() const = 0;
  };

}}} // ts::appfw::gl
#endif
