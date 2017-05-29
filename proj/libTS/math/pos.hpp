/** pos.hpp -*-tab-width:4;mode:C++-*-





*/


#ifndef TS_MATH_POS_HPP
#define TS_MATH_POS_HPP

#include <math.h>
#ifdef WIN32
#include "../Common/StdTypes.h"
#else
#include <stdint.h>
#endif
#include <utility>
#include <algorithm>
#include "arithmetic_operations.hpp"

namespace ts {
namespace math {
  
	
	
  template <typename T>
  struct pos2 : arithmetic_operations2<pos2<T>, T>{
	typedef T value_type;
	T x_;
	T y_;
	pos2() : x_(0), y_(0) {}
	pos2(const pos2& pos) : x_(pos.x_), y_(pos.y_) {}
	template <typename TT>
	pos2(TT x, TT y) : x_(static_cast<T>(x)), y_(static_cast<T>(y)) {}
	template <typename TT>
	pos2(const TT& pos) 
	  : x_(static_cast<T>(pos.x()))
	  , y_(static_cast<T>(pos.y())) 
	{}

	T x() const { return x_; }
	T y() const { return y_; }
	
	void swap(pos2& s) throw() { std::swap(x_, s.x_); std::swap(y_, s.y_); }
	// 要求型に自動変換する
	template <typename TT> operator pos2<TT> () { return pos2<TT>(x_, y_); }

	// 距離を返す
	T distance(const pos2& pos) const {
	  pos2 p(pos - *this);
	  return sqrt(p.x_*p.x_ + p.y_*p.y_);
	}
  };

  template <typename T>
  struct pos3 : arithmetic_operations3<pos3<T>, T>{
	typedef T value_type;
	T x_;
	T y_;
	T z_;
	pos3() : x_(0), y_(0), z_(0) {}
	pos3(const pos3& pos) : x_(pos.x_), y_(pos.y_), z_(pos.z_) {}
	template <typename TT>
	pos3(TT x, TT y, TT z) : x_(static_cast<T>(x)), y_(static_cast<T>(y)), z_(static_cast<T>(z)) {}
	template <typename TT>
	pos3(const TT& pos) : x_(static_cast<T>(pos.x_)), y_(static_cast<T>(pos.y_)), z_(static_cast<T>(pos.z_)) {}
	
	T x() const { return x_; }
	T y() const { return y_; }
	T z() const { return z_; }

	void swap(pos3& s) throw() { swap(x_, s.x_); swap(y_, s.y_); swap(z_, s.z_);  }
	// 要求型に自動変換する
	template <typename TT> operator pos3<TT> () { return pos3<TT>(x_,y_,z_); }
	// 距離を返す
	T distance(const pos3& pos) const {
	  pos3 p(pos - *this);
	  return sqrt(p.x_*p.x_ + p.y_*p.y_* + p.z_*p.z_);
	}
  };

  template <typename T> inline void swap(pos2<T>& p1, pos2<T>& p2) { p1.swap(p2); }

  typedef pos2<double> pos2_d;
  typedef pos2<float> pos2_f;
  typedef pos2<int32_t> pos2_i;
  typedef pos2<uint32_t> pos2_ui;
  typedef pos3<double> pos3_d;
  typedef pos3<float> pos3_f;
  typedef pos3<int32_t> pos3_i;


  // x_, y_を入れ替えるためのクラス
  // 例
  // pos2_f pos;
  // swapper2 swp(false);
  // swp(pos).x_ はpos.y_を示す!
  struct swapper2 {
	template <typename T>
	struct result {
	  typedef typename T::value_type VT;
	  VT& x_;
	  VT& y_;
	  result(VT& x, VT& y) : x_(x), y_(y) {}
	};

	bool noswap_;
	swapper2(bool noswp) : noswap_(noswp) {}

	operator bool () const { return noswap_; }

	template <typename T>
	result<T> operator () (T& p) const {
	  return noswap_ ? result<T>(p.x_, p.y_) : result<T>(p.y_, p.x_);
	}

	// noswap_の条件でaかbを返す!
	template <typename T>
	T& operator () (T& a, T& b) { return noswap_ ? a : b; }
	
  };

  template <typename T> inline void swap(pos3<T>& p1, pos3<T>& p2) { p1.swap(p2); }
  
  template <typename T> inline pos2<T> pos(T x, T y) { return pos2<T>(x,y); }
  template <typename T> inline pos3<T> pos(T x, T y, T z) { return pos3<T>(x,y,z); }

}}
#endif

