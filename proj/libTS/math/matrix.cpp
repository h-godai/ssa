#include <math.h>
#include "matrix.hpp"
#include <algorithm>

using std::swap;

// テンプレート用のオブジェクトを生成させる
namespace {
  using namespace ts;
  using namespace ts::math;
  struct hoge {
    Matrix mat;
    Matrixd matd;
    hoge() {
      vec3_f obj;
      vec3_d objd;
      mat.unProject(0.0f, 0.0f, 0.0f, mat, rect_i(), obj);
      matd.unProject(0.0, 0.0, 0.0, matd, rect_i(), objd);
    }
  } hohoho;
}


namespace ts {


  // 逆行列
  template <typename ValueType>
  bool MatrixT<ValueType>::invert_matrix(const MatrixT& m, MatrixT& out)
    {

      ValueType	wtmp[4][8];
      ValueType	m0, m1, m2, m3, s;
      ValueType	*r0, *r1, *r2, *r3;

      r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

      r0[0] = m.get(0, 0), r0[1] = m.get(0, 1),
      r0[2] = m.get(0, 2), r0[3] = m.get(0, 3),
      r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
      r1[0] = m.get(1, 0), r1[1] = m.get(1, 1),
      r1[2] = m.get(1, 2), r1[3] = m.get(1, 3),
      r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
      r2[0] = m.get( 2, 0), r2[1] = m.get(2, 1),
      r2[2] = m.get( 2, 2), r2[3] = m.get(2, 3),
      r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
      r3[0] = m.get( 3, 0), r3[1] = m.get( 3, 1),
      r3[2] = m.get( 3, 2), r3[3] = m.get( 3, 3),
      r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

      /* choose pivot - or die */
      if (fabs(r3[0]) > fabs(r2[0]))      swap(r3, r2);
      if (fabs(r2[0]) > fabs(r1[0]))      swap(r2, r1);
      if (fabs(r1[0]) > fabs(r0[0]))      swap(r1, r0);
      if (0.0 == r0[0]) return false;

      m1 = r1[0] / r0[0];
      m2 = r2[0] / r0[0];
      m3 = r3[0] / r0[0];
      s = r0[1];
      r1[1] -= m1 * s;
      r2[1] -= m2 * s;
      r3[1] -= m3 * s;
      s = r0[2];
      r1[2] -= m1 * s;
      r2[2] -= m2 * s;
      r3[2] -= m3 * s;
      s = r0[3];
      r1[3] -= m1 * s;
      r2[3] -= m2 * s;
      r3[3] -= m3 * s;
      s = r0[4];
      if (s != 0.0) {
        r1[4] -= m1 * s;
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
      }
      s = r0[5];
      if (s != 0.0) {
        r1[5] -= m1 * s;
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
      }
      s = r0[6];
      if (s != 0.0) {
        r1[6] -= m1 * s;
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
      }
      s = r0[7];
      if (s != 0.0) {
        r1[7] -= m1 * s;
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
      }

      if (fabs(r3[1]) > fabs(r2[1]))        swap(r3, r2);
      if (fabs(r2[1]) > fabs(r1[1]))        swap(r2, r1);
      if (0.0 == r1[1])        return false;

      m2 = r2[1] / r1[1];
      m3 = r3[1] / r1[1];
      r2[2] -= m2 * r1[2];
      r3[2] -= m3 * r1[2];
      r2[3] -= m2 * r1[3];
      r3[3] -= m3 * r1[3];
      s = r1[4];
      if (0.0 != s) {
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
      }
      s = r1[5];
      if (0.0 != s) {
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
      }
      s = r1[6];
      if (0.0 != s) {
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
      }
      s = r1[7];
      if (0.0 != s) {
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
      }

      if (fabs(r3[2]) > fabs(r2[2])) swap(r3, r2);
      if (0.0 == r2[2]) return false;

      m3 = r3[2] / r2[2];
      r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
      r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

      if (0.0f == r3[3]) return false;

      s = 1.0f / r3[3];
      r3[4] *= s;
      r3[5] *= s;
      r3[6] *= s;
      r3[7] *= s;

      m2 = r2[3];
      s = 1.0f / r2[2];
      r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
        r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
      m1 = r1[3];
      r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
        r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
      m0 = r0[3];
      r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
        r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

      m1 = r1[2];
      s = 1.0f / r1[1];
      r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
        r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
      m0 = r0[2];
      r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
        r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

      m0 = r0[1];
      s = 1.0f / r0[0];
      r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
        r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

      out.at( 0, 0) = r0[4];
      out.at( 0, 1) = r0[5]; out.at( 0, 2) = r0[6];
      out.at( 0, 3) = r0[7]; out.at( 1, 0) = r1[4];
      out.at( 1, 1) = r1[5]; out.at( 1, 2) = r1[6];
      out.at( 1, 3) = r1[7]; out.at( 2, 0) = r2[4];
      out.at( 2, 1) = r2[5]; out.at( 2, 2) = r2[6];
      out.at( 2, 3) = r2[7]; out.at( 3, 0) = r3[4];
      out.at( 3, 1) = r3[5]; out.at( 3, 2) = r3[6];
      out.at( 3, 3) = r3[7];

   return true;
  }

  template <typename ValueType>
  void MatrixT<ValueType>::transform_point(ValueType out[4], const MatrixT& m, const ValueType in[4])
  {
     out[0] = m.get(0, 0) * in[0] + m.get(0, 1) * in[1] + m.get(0, 2) * in[2] + m.get(0, 3) * in[3];
     out[1] = m.get(1, 0) * in[0] + m.get(1, 1) * in[1] + m.get(1, 2) * in[2] + m.get(1, 3) * in[3];
     out[2] = m.get(2, 0) * in[0] + m.get(2, 1) * in[1] + m.get(2, 2) * in[2] + m.get(2, 3) * in[3];
     out[3] = m.get(3, 0) * in[0] + m.get(3, 1) * in[1] + m.get(3, 2) * in[2] + m.get(3, 3) * in[3];
  }


}
