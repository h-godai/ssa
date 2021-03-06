// -*-tab-width:4-*-
// @brief 平方根を求める


#ifndef TS_PIML_SQRT_HPP
#define TS_PIML_SQRT_HPP

#ifndef TS_PIML_HPP
#include "piml.hpp"
#endif

namespace piml {

  namespace mpl {
    // 2のN乗を求める
    template <int N>
    struct pow2 {
      enum { value = pow2<N-1>::value * 2 };
    };
    template <>
    struct pow2<0> {
      enum { value = 1 };
    };
    // Nが2の?乗を求める
    template <int N, int A2, int A>
    struct rpow2_sub {
      enum { value = rpow2_sub<N, pow2<A>::value, A+1>::value };
    };
    template <int N, int A>
    struct rpow2_sub<N, N, A> {
      enum { value = A-1 };
    };
    template <int N>
    struct rpow2 {
      enum { value = rpow2_sub<N, 1, 0>::value };
    };
    // Nの平方根を求める。ただしNは2のべき乗のみ
    template <int N, int A2, int A>
    struct sqrt2_impl {
      enum { value = sqrt2_impl<N, A*A, A*2>::value };
    };
    template <int A2, int A>
    struct sqrt2_impl<A2, A2, A> {
      enum { value = A/2 };
    };
    template <size_t N>
    struct sqrt2 {
      enum { value = sqrt2_impl<N, 2, 1>::value };
    };
  } // mpl

  namespace detail {
    // 平方根を求めるサブクラス
    template <int Loop>
    struct sqrt_sub {
      template < typename ValueType> static ValueType calc(ValueType c) {
        ValueType xn = (c + static_cast<ValueType>(1))  * static_cast<ValueType>(0.5); // loop1
        for (int loop = 0; loop < Loop-1; ++loop) {
          xn = (xn + (c/xn)) / static_cast<ValueType>(2);
        }
        return xn;
      }
    };
    template <> struct sqrt_sub<2> {
      template < typename ValueType> static ValueType calc(ValueType c) {
        ValueType xn = (c + static_cast<ValueType>(1))  * static_cast<ValueType>(0.5); // loop1
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        return xn;
      }
    };
    template <> struct sqrt_sub<3> {
      template < typename ValueType> static ValueType calc(ValueType c) {
        ValueType xn = (c + static_cast<ValueType>(1)) * static_cast<ValueType>(0.5); // loop1
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        return xn;
      }
    };
    template <> struct sqrt_sub<4> {
      template < typename ValueType> static ValueType calc(ValueType c) {
        ValueType xn = (c + static_cast<ValueType>(1)) * static_cast<ValueType>(0.5); // loop1
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        return xn;
      }
    };
    template <> struct sqrt_sub<5> {
      template < typename ValueType>
      static ValueType calc(ValueType c) {
        ValueType xn = (c + static_cast<ValueType>(1)) * static_cast<ValueType>(0.5); // loop1
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        xn = (xn + (c/xn)) * static_cast<ValueType>(0.5);
        return xn;
      }
    };
  } // detail

  // ニュートン・ラプソン法
  // 誤差の少ない範囲に補正して計算する
  //
  // 基数 補正  境界      LOOP  最大誤差
  // 8    x16   1/256〜256   8  0.000024% (400万分の1以下)
  // 8    x16   1/256〜256   7  0.066391%
  // 8    x16   1/256〜256   6  0.066391%                   0.0662μ秒
  // 6    x8    1/64〜64     6  0.000024% (400万分の1以下)
  // 6    x8    1/64〜64     5  0.06435%                    0.0606μ秒
  // 4    x4    1/16〜16     6  0.0% (誤差を検出できず)
  // 4    x4    1/16〜16     5  0.000024% (400万分の1以下)  0.0620μ秒
  // 4    x4    1/16〜16     4  0.0564%                     0.0464μ秒
  // 2    x2    1/4〜4       5  0.0% (誤差を検出できず)     0.0766μ秒
  // 2    x2    1/4〜4       4  0.000012% (800万分の1以下)  0.0656μ秒
  // 2    x2    1/4〜4       3  0.030494%                   0.0509μ秒
  template <int Base, int Loop, typename ValueType>
  ValueType sqrt_t(ValueType c) {
    static const int iCorrectRatio         = mpl::pow2<Base>::value; // 2^Base
    static const int SqrtRatioSB           = mpl::rpow2<mpl::sqrt2<iCorrectRatio>::value>::value;    // 計算後に戻すための値の計算用
    static const ValueType CorrectRatio    = static_cast<ValueType>(iCorrectRatio);
    static const ValueType CorrectRatioInv = static_cast<ValueType>(1.0/iCorrectRatio);
    static const ValueType SmallVal        = static_cast<ValueType>(1)/CorrectRatio;
    using piml::detail::sqrt_sub;
    if (c == 0) return static_cast<ValueType>(0); // 高速化のため、最小範囲での比較はしない
    if (c > CorrectRatio) {
      unsigned n = 1;
      do {
        c *= CorrectRatioInv;
        n <<= SqrtRatioSB;
      } while (c > CorrectRatio);
      return sqrt_sub<Loop>::calc(c) * n;
    }
    if (c < SmallVal) {
      unsigned s = 1;
      do {
        c *= CorrectRatio;
        s <<= SqrtRatioSB;
      } while (c < SmallVal);
      return sqrt_sub<Loop>::calc(c) / s;
    }
    return sqrt_sub<Loop>::calc(c);
  }

  // 基数とループの初期値
  const int DefaultSqrtBase = 4;
  const int DefaultSqrtLoop = 4;

  // デフォルト値でのsqrt
  template <typename ValueType>
  ValueType sqrt(ValueType c) { return sqrt_t<DefaultSqrtBase, DefaultSqrtLoop>(c); }

  // 引数が1.0付近に最適化されている場合のsqrt 平均2倍程度高速です
  template <typename ValueType>
  ValueType sqrt_normal(ValueType c) { return piml::detail::sqrt_sub<4>::calc(c); }
}

#endif
