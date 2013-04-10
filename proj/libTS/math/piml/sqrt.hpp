// -*-tab-width:4-*-
// @brief �����������߂�


#ifndef TS_PIML_SQRT_HPP
#define TS_PIML_SQRT_HPP

#ifndef TS_PIML_HPP
#include "piml.hpp"
#endif

namespace piml {

  namespace mpl {
    // 2��N������߂�
    template <int N>
    struct pow2 {
      enum { value = pow2<N-1>::value * 2 };
    };
    template <>
    struct pow2<0> {
      enum { value = 1 };
    };
    // N��2��?������߂�
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
    // N�̕����������߂�B������N��2�ׂ̂���̂�
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
    // �����������߂�T�u�N���X
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

  // �j���[�g���E���v�\���@
  // �덷�̏��Ȃ��͈͂ɕ␳���Čv�Z����
  //
  // � �␳  ���E      LOOP  �ő�덷
  // 8    x16   1/256�`256   8  0.000024% (400������1�ȉ�)
  // 8    x16   1/256�`256   7  0.066391%
  // 8    x16   1/256�`256   6  0.066391%                   0.0662�ʕb
  // 6    x8    1/64�`64     6  0.000024% (400������1�ȉ�)
  // 6    x8    1/64�`64     5  0.06435%                    0.0606�ʕb
  // 4    x4    1/16�`16     6  0.0% (�덷�����o�ł���)
  // 4    x4    1/16�`16     5  0.000024% (400������1�ȉ�)  0.0620�ʕb
  // 4    x4    1/16�`16     4  0.0564%                     0.0464�ʕb
  // 2    x2    1/4�`4       5  0.0% (�덷�����o�ł���)     0.0766�ʕb
  // 2    x2    1/4�`4       4  0.000012% (800������1�ȉ�)  0.0656�ʕb
  // 2    x2    1/4�`4       3  0.030494%                   0.0509�ʕb
  template <int Base, int Loop, typename ValueType>
  ValueType sqrt_t(ValueType c) {
    static const int iCorrectRatio         = mpl::pow2<Base>::value; // 2^Base
    static const int SqrtRatioSB           = mpl::rpow2<mpl::sqrt2<iCorrectRatio>::value>::value;    // �v�Z��ɖ߂����߂̒l�̌v�Z�p
    static const ValueType CorrectRatio    = static_cast<ValueType>(iCorrectRatio);
    static const ValueType CorrectRatioInv = static_cast<ValueType>(1.0/iCorrectRatio);
    static const ValueType SmallVal        = static_cast<ValueType>(1)/CorrectRatio;
    using piml::detail::sqrt_sub;
    if (c == 0) return static_cast<ValueType>(0); // �������̂��߁A�ŏ��͈͂ł̔�r�͂��Ȃ�
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

  // ��ƃ��[�v�̏����l
  const int DefaultSqrtBase = 4;
  const int DefaultSqrtLoop = 4;

  // �f�t�H���g�l�ł�sqrt
  template <typename ValueType>
  ValueType sqrt(ValueType c) { return sqrt_t<DefaultSqrtBase, DefaultSqrtLoop>(c); }

  // ������1.0�t�߂ɍœK������Ă���ꍇ��sqrt ����2�{���x�����ł�
  template <typename ValueType>
  ValueType sqrt_normal(ValueType c) { return piml::detail::sqrt_sub<4>::calc(c); }
}

#endif
