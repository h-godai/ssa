// �O�p�֐�


#ifndef TS_PIML_TRIGONOMETRIC_HPP
#define TS_PIML_TRIGONOMETRIC_HPP

#ifndef TS_PIML_HPP
#include "piml.hpp"
#endif
#include <assert.h>

namespace piml {

  const float FloatMax = 3.402823e+38f;
  const float PIf = 3.14159265358979f;
  const double PId = 3.14159265358979323846264338328f;
  const float Inv2PI = 1.0f / (2.0f * PIf); // 1/2��
  const float Inv360 = 1.0f / 360.0f;

  // ���W�A���l 0-2��
  struct radian {
    float value_;
    radian(float r = 0.0f) : value_(r) {}
    void operator = (float r) { value_ = r; }
  };
  // �f�V�}���l 0-360
  struct decimal {
    float value_;
    decimal(float d = 0.0f) : value_(d) {}
    void operator = (float d) { value_ = d; }
  };

  // �p�x��\�����邽�߂̃N���X
  // BITS�͕���\ (1<<BITS) ��2�΂ƂȂ�
  template <int BITS>
  struct theta {
    static const size_t Resolution = 1 << BITS;  // 0�`2�΂܂ł̕���\
    int32_t theta_;   // 32bit �Œ菬���_ ����16bit��2�΂ƂȂ�
    theta(uint32_t t = 0)     : theta_(t) {}
    theta(radian r)  : theta_(radian_to_theta(r.value_)) {}
    theta(decimal d) : theta_(deciaml_to_theta(d.value_)) {}

    void operator = (const radian& r) { theta_ = radian_to_theta(r.value_); }

    theta operator + (theta t) const { return theta_ + t.theta_; }
    theta operator - (theta t) const { return theta_ - t.theta_; }
    theta operator += (theta t) const { theta_ += t.theta_; return theta_; }
    theta operator -= (theta t) const { theta_ -= t.theta_; return theta_; }

    operator radian () const {
      return theta_ * PIf * 2.0f / Resolution;
    }
    operator decimal () const {
      return theta_ * 360.0f / Resolution;
    }

    static int32_t radian_to_theta(float r)  { 
      return static_cast<int32_t>(r * Inv2PI * Resolution + 0.5f); // & 0xffff;  
    }
    static int32_t decimal_to_theta(float r) { 
      return static_cast<int32_t>(r * Inv360 * Resolution + 0.5f); // & 0xffff; 
    }



    template <int Reso>
    uint32_t get() const {
      static const uint32_t mask = (1 << (Reso)) - 1;
      return (theta_ >> (BITS - Reso)) & mask;
    }
  private:
    theta(float);
    void operator = (float);
  };

  // �e�[�u���Q�Ƃɂ��sin/cos/tan
  // ����\ ���ό덷 �ő�덷
  // 10bit  0.19%    0.59%
  // 11bit  0.09%    0.29%
  template <int BITS>
  struct sin_cos_table {
      static const size_t Resolution = 1 << BITS;  // 0�`2�΂܂ł̕���\
      static const size_t TableSize = 1 << (BITS-1); // �e�[�u���̃T�C�Y
      static const size_t TableMask = TableSize - 1;
      static const float sinTable_[TableSize];
      template <size_t TBIT>
      static inline float sin(const theta<TBIT>& th) {
        return value(Resolution + th.get<BITS>());
      }
      template <size_t TBIT>
      static inline float cos(theta<TBIT> th) {
        return value(Resolution/4 + Resolution + th.get<BITS>());
      }
      template <size_t TBIT>
      static inline void sincos(theta<TBIT> th, float& s, float& c) {
          int32_t t =  Resolution + th.get<BITS>(); // static_cast<int>(rad * ( Resolution / (3.14159265f * 2.0f)));
          s = value(t);
          c = value(t + Resolution/4);
      }
      template <size_t TBIT>
      static inline float tan(theta<TBIT> th) {
        float s, c;
        sincos(th, s, c);
        if (c == 0.0f) return FloatMax;
        return s/c;
      }

  private:
        static inline float value(int32_t th) {
          if (th & TableSize) {
              return -sinTable_[th & TableMask];
          }
          else {
              return sinTable_[th & TableMask];
          }
      }
  public:
      template <typename OutStream>
      static void make_table(OutStream&);
  };

  // �A�[�N�T�C��
  template <int BITS>
  struct asin_table {
    static const size_t Resolution = 1 << BITS;  // ����\
    static const float asinTable_[Resolution];

    template <typename OutStream>
    static void make_table(OutStream&);
    static float arc_sin(float x) {
      assert(x >= 0.0f && x <= 1.0f);
      size_t index = static_cast<size_t>(x * Resolution);
      if (index == Resolution) return PIf/2.0f;
      if (index == Resolution-1) return asinTable_[index];
      float xx = 1.0f / index;
      float ratio = (x - xx)*Resolution;
      return (asinTable_[index] * (1.0f-ratio)) + (asinTable_[++index] * ratio);
    }
    static float arc_cos(float x);
  };

  // arctan ��^�ʖڂɌv�Z����B
  // Taylor�W�J(Maclaurin�W�J) 
  float arctan(float xx);

  // atan2
  // vector(x,y)�𐳋K�����Ax�̒l�ɑ΂���arccos���s��
  // arccos��table lookup
  float atan2(float x, float y);

}
#endif

