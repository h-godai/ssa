// -*-tab-width:4-*-
// @brief •½•ûª‚ğ‹‚ß‚é


#ifndef TS_PIML_SQRT_HPP
#define TS_PIML_SQRT_HPP

#ifndef TS_PIML_HPP
#include "piml.hpp"
#endif

namespace piml {

  namespace mpl {
    // 2‚ÌNæ‚ğ‹‚ß‚é
    template <int N>
    struct pow2 {
      enum { value = pow2<N-1>::value * 2 };
    };
    template <>
    struct pow2<0> {
      enum { value = 1 };
    };
    // N‚ª2‚Ì?æ‚ğ‹‚ß‚é
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
    // N‚Ì•½•ûª‚ğ‹‚ß‚éB‚½‚¾‚µN‚Í2‚Ì‚×‚«æ‚Ì‚İ
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
    // •½•ûª‚ğ‹‚ß‚éƒTƒuƒNƒ‰ƒX
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

  // ƒjƒ…[ƒgƒ“Eƒ‰ƒvƒ\ƒ“–@
  // Œë·‚Ì­‚È‚¢”ÍˆÍ‚É•â³‚µ‚ÄŒvZ‚·‚é
  //
  // Šî” •â³  ‹«ŠE      LOOP  Å‘åŒë·
  // 8    x16   1/256`256   8  0.000024% (400–œ•ª‚Ì1ˆÈ‰º)
  // 8    x16   1/256`256   7  0.066391%
  // 8    x16   1/256`256   6  0.066391%                   0.0662ƒÊ•b
  // 6    x8    1/64`64     6  0.000024% (400–œ•ª‚Ì1ˆÈ‰º)
  // 6    x8    1/64`64     5  0.06435%                    0.0606ƒÊ•b
  // 4    x4    1/16`16     6  0.0% (Œë·‚ğŒŸo‚Å‚«‚¸)
  // 4    x4    1/16`16     5  0.000024% (400–œ•ª‚Ì1ˆÈ‰º)  0.0620ƒÊ•b
  // 4    x4    1/16`16     4  0.0564%                     0.0464ƒÊ•b
  // 2    x2    1/4`4       5  0.0% (Œë·‚ğŒŸo‚Å‚«‚¸)     0.0766ƒÊ•b
  // 2    x2    1/4`4       4  0.000012% (800–œ•ª‚Ì1ˆÈ‰º)  0.0656ƒÊ•b
  // 2    x2    1/4`4       3  0.030494%                   0.0509ƒÊ•b
  template <int Base, int Loop, typename ValueType>
  ValueType sqrt_t(ValueType c) {
    static const int iCorrectRatio         = mpl::pow2<Base>::value; // 2^Base
    static const int SqrtRatioSB           = mpl::rpow2<mpl::sqrt2<iCorrectRatio>::value>::value;    // ŒvZŒã‚É–ß‚·‚½‚ß‚Ì’l‚ÌŒvZ—p
    static const ValueType CorrectRatio    = static_cast<ValueType>(iCorrectRatio);
    static const ValueType CorrectRatioInv = static_cast<ValueType>(1.0/iCorrectRatio);
    static const ValueType SmallVal        = static_cast<ValueType>(1)/CorrectRatio;
    using piml::detail::sqrt_sub;
    if (c == 0) return static_cast<ValueType>(0); // ‚‘¬‰»‚Ì‚½‚ßAÅ¬”ÍˆÍ‚Å‚Ì”äŠr‚Í‚µ‚È‚¢
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

  // Šî”‚Æƒ‹[ƒv‚Ì‰Šú’l
  const int DefaultSqrtBase = 4;
  const int DefaultSqrtLoop = 4;

  // ƒfƒtƒHƒ‹ƒg’l‚Å‚Ìsqrt
  template <typename ValueType>
  ValueType sqrt(ValueType c) { return sqrt_t<DefaultSqrtBase, DefaultSqrtLoop>(c); }

  // ˆø”‚ª1.0•t‹ß‚ÉÅ“K‰»‚³‚ê‚Ä‚¢‚éê‡‚Ìsqrt •½‹Ï2”{’ö“x‚‘¬‚Å‚·
  template <typename ValueType>
  ValueType sqrt_normal(ValueType c) { return piml::detail::sqrt_sub<4>::calc(c); }
}

#endif
