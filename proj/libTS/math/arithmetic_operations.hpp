/** arithmetic_operations.hpp -*-tab-width:4;mode:C++-*-





*/


#ifndef TS_MATH_ARITHMETIC_OPERATIONS_HPP
#define TS_MATH_ARITHMETIC_OPERATIONS_HPP


namespace ts {
namespace math {

  template <typename C>
  struct basic_operation {
    C& operator = (C s) {
      static_cast<C&>(*this).swap(s);
      return static_cast<C&>(*this);
    }
    void clear() {
      C tmp;
      static_cast<C&>(*this).swap(tmp);
    }
  };

  template <typename C, typename VT>
  struct arithmetic_operations2 : basic_operation<C> {
    typedef const C CC;
    C operator + (C p) const { CC& t = static_cast<CC&>(*this); p.x_ += t.x_; p.y_ += t.y_; return p; }
    C operator - (C p) const { CC& t = static_cast<CC&>(*this); p.x_ = t.x_ - p.x_; p.y_ = t.y_ - p.y_; return p; }
    C operator / (VT d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ / d, t.y_ / d); }
    C operator / (CC& d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ / d.x_, t.y_ / d.y_); }
    C operator * (VT d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ * d, t.y_ * d); }
    C operator * (CC& d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ * d.x_, t.y_ * d.y_); }
    C& operator /= (VT d) { C& t = static_cast<C&>(*this); t.x_ /= d; t.y_ /= d; return t; }
    C& operator *= (VT d) { C& t = static_cast<C&>(*this); t.x_ *= d; t.y_ *= d; return t; }

    template <typename O>
    C& operator +=(const O& o) { C& t = static_cast<C&>(*this); t.x_ += o.x_; t.y_ += o.y_; return t;}
    template <typename O>
    C& operator -=(const O& o) { C& t = static_cast<C&>(*this); t.x_ -= o.x_; t.y_ -= o.y_; return t; }

	bool operator == (CC& o) const { CC& t = static_cast<CC&>(*this); return t.x_ == o.x_ &&  t.y_ == o.y_; }
	bool operator != (CC& o) const { CC& t = static_cast<CC&>(*this); return t.x_ != o.x_ ||  t.y_ != o.y_; }
      
  };

  template <typename C, typename VT>
  struct arithmetic_operations3 : basic_operation<C> {
    typedef const C CC;
    C operator + (C p) const { CC& t = static_cast<CC&>(*this); p.x_ += t.x_; p.y_ += t.y_; p.z_ += t.z_; return p; }
    C operator - (C p) const { CC& t = static_cast<CC&>(*this); p.x_ = t.x_ - p.x_; p.y_ = t.y_ - p.y_; p.z_ = t.z_ - p.z_; return p; }
    C operator / (VT d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ / d, t.y_ / d, t.z_ / d); }
    C operator * (VT d) const { CC& t = static_cast<CC&>(*this); return C(t.x_ * d, t.y_ * d, t.z_ * d); }
    C& operator /= (VT d) { C& t = static_cast<C&>(*this); t.x_ /= d; t.y_ /= d; t.z_ /= d; return t; }
    C& operator *= (VT d) { C& t = static_cast<C&>(*this); t.x_ *= d; t.y_ *= d; t.z_ *- d; return t; }

    template <typename O>
    C& operator +=(const O& o) { C& t = static_cast<C&>(*this); t.x_ += o.x_; t.y_ += o.y_; t.z_ += o.z_; return t;}
    template <typename O>
    C& operator -=(const O& o) { C& t = static_cast<C&>(*this); t.x_ -= o.x_; t.y_ -= o.y_; t.z_ -= o.z_; return t; }
      
  };

}
}
#endif
