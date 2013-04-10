/** rect.hpp -*-tab-width:4;mode:C++-*-





*/


#ifndef TS_MATH_RECT_HPP
#define TS_MATH_RECT_HPP

//#include "libTS/Common/Common.h"
#include "pos.hpp"
#include "size.hpp"

namespace ts {
namespace math {


  template <typename T>
  struct rect {
	typedef size_T<T> size_type;
	typedef pos2<T> pos_type;
	pos_type pos_;
	size_type size_;
	rect() {}
	rect(const pos_type& pos, const size_type& size = size_type())
	  : pos_(pos)
	  , size_(size)
	{}
	template <typename TT>
    rect(TT l, TT t, TT w, TT h)
	  : pos_(static_cast<T>(l), static_cast<T>(t))
	  , size_(static_cast<T>(w), static_cast<T>(h))
	{}

	T left() const { return pos_.x_; }
	T top() const { return pos_.y_; }
	T right() const { return pos_.x_ + size_.width_; }
	T bottom() const { return pos_.y_ + size_.height_; }
	T width() const { return size_.width_; }
	T height() const { return size_.height_; }

	T cx() const { return (left() + right()) / 2; }
	T cy() const { return (top() + bottom()) / 2; }
	pos2<T> center() const {
	  T x = cx();
	  T y = cy();
	  return pos2<T>(x,y);
	}

	T x() const { return pos_.x_; }
	T y() const { return pos_.y_; }


	void set_size(size_type size) {
	  size_.swap(size);
	}
	const size_type& size() const { return size_; }

	// pがrectの内側にあるか調べる!
	template <typename PosT>
	bool is_inside(const PosT& p) const {
	  return p.x_ >= left() && p.x_ < right() && p.y_ >= top() && p.y_ < bottom();
	}

	bool empty() const { return size_.width_ == 0 && size_.height_ == 0; }

    // widthとheightが正になるよう調整する!
    void correct() {
      if (size_.width_ < 0) { pos_.x_ -= size_.width_; size_.width_ = -size_.width_; }
      if (size_.height_ < 0) { pos_.y_  -= size_.height_; size_.height_ = -size_.height_; }
    }

	operator pos_type() {
	  return pos_;
	}

	// sz分だけrectを大きくする!
	rect operator + (const size_type& sz) const {
	  return rect(pos_ - (sz/2), size_ + sz);
	}
	// sz分だけrectを小さくする!
	rect operator - (const size_type& sz) const {
	  return rect(pos_ + (sz/2), size_ + sz);
	}

	// 縦横別の拡大・縮小!
	template <typename St>
	rect operator * (const St& k) const {
	  return rect(pos_.x_ * k.x_, pos_.y_ * k.y_, size_.width_ * k.x_, size_.height_ * k.y_);
	}

	void swap(rect& s) throw() {
	  pos_.swap(s.pos_);
	  size_.swap(s.size_);
	}
  };


  typedef rect<float> rect_f;
  typedef rect<int32_t> rect_i;

  template <typename T>
  inline pos2<T> clop(const pos2<T>& pos, const rect<T>& r, T resolution=1) {
	pos2<T> cloped(pos);
	if (cloped.x_ >= r.right()) cloped.x_ = r.right() - resolution;
	if (cloped.y_ >= r.bottom()) cloped.y_ = r.bottom() - resolution;
	if (cloped.x_ < r.left()) cloped.x_ = r.left();
	if (cloped.y_ < r.top()) cloped.y_ = r.top();
	return cloped;
  }


}} // namespace

#endif

