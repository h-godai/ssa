/** Rect.h -*-C++-*-





*/


#ifndef TS_Rect_h
#define TS_Rect_h


#include "libTS/Common/Common.h"
#include "Vector.h"

namespace ts {

  template <typename T>
  struct Rect {
    T left;
    T top;
    T width;
    T height;
    Rect() : left(0), top(0), width(0), height(0) {}
    template <typename TT>
    Rect(TT l, TT t, TT w, TT h) : left(static_cast<T>(l)), top(static_cast<T>(t)), width(static_cast<T>(w)), height(static_cast<T>(h)) {}
    Rect(const Rect& rect) : top(rect.top), left(rect.left), width(rect.width), height(rect.height) {}
    template <typename R>
    Rect(const Rect<R>& rect) 
      : top(static_cast<T>(rect.top))
      , left(static_cast<T>(rect.left))
      , width(static_cast<T>(rect.width))
      , height(static_cast<T>(rect.height)) {}
    T bottom() const { return top + height; }
    T right() const { return left + width; }
    T cx() const { return (left + left + width) / 2; }
    T cy() const { return (top + top + height) / 2; }
    bool isInside(const Vector2DT<T>& v) const { return isInside(v.x, v.y); }
    bool isInside(T x, T y) const {
      if (x >= left && x < left + width && y >= top && y < top + height) return true;
      return false;
    }
    bool isEmpty() const { return width == 0 && height == 0; }
    void clear() { left = top = width = height = 0; }
    // width‚Æheight‚ª³‚É‚È‚é‚æ‚¤’²®‚·‚é
    void correct() { 
      if (width < 0) { left -= width; width = -width; }
      if (height < 0) { top -= height; height = -height; }
    }
  };

  typedef Rect<int32_t> RectS32;
  typedef Rect<float32_t> RectF32;
  typedef Rect<float64_t> RectF64;

  typedef Rect<int32_t> Recti;
  typedef Rect<float32_t> Rectf;
  typedef Rect<float64_t> Rectd;


}

#endif

