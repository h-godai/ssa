/*

*/

#ifndef TSGRAPHICS_MATRIX_H
#define TSGRAPHICS_MATRIX_H

#include <math.h>
#include <boost/foreach.hpp>
#include "libTS/Common/Common.h"
#include "Vector.h"
#include "Rect.h"

namespace ts {

  const double Pai = 3.14159265358979323846264338;
  const double Pai2Rad = (2.0 * Pai) / 360.0;
  const double Rad2Pai = 360.0 / (2.0 * Pai);
  const float Paif = 3.14159265358979323846264338f;
  const float Pai2Radf = (2.0f * Paif) / 360.0f;
  const float Rad2Paif = 360.0f / (2.0f * Paif);

  namespace math {
    enum RowMajorMatrix {
      rm11,rm21,rm31,rm41,
      rm12,rm22,rm32,rm42,
      rm13,rm23,rm33,rm43,
      rm14,rm24,rm34,rm44
    };
  }

  /// Matrixの定義
  template <typename ValueType>
  struct MatrixT {
    /* 並び順 row major
        0:_11   4:_12   8:_13  12:_14
        1:_21   5:_22   9:_23  13:_24
        2:_31   6:_32  10:_33  14:_34
        3:_41   7:_42  11:_43  15:_44
    */

    ValueType m[16];

    enum AngleType {
        Angle, // 度 (0-360)
        Radian // ラジアン値 (0-2pai)
    };

    MatrixT() { identity(); }
    MatrixT(const MatrixT& src) {
      memcpy(m, src.m, sizeof(m));
    }
    // ValueType*からマトリックスを作る
    explicit MatrixT(const ValueType* src) { 
      memcpy(m, src, sizeof(m));
    }
    // translateからMatrixを作成する
    explicit MatrixT(const Vector3DT<ValueType>& v) {
      translate(v);
    }
    ValueType operator [] (math::RowMajorMatrix rm) {
      return m[rm];
    }

    // get(row, column)
    ValueType& at(unsigned int row, unsigned int column) {
      return m[(column * 4) + row];
    }
    const ValueType& at(unsigned int row, unsigned int column) const {
      return m[(column * 4) + row];
    }
    const ValueType& get(unsigned int row, unsigned int column) const {
      return m[(column * 4) + row];
    }

    void identity() {
      memset(m, 0, sizeof(m));
      at(0,0) = static_cast<ValueType>(1.0);
      at(1,1) = static_cast<ValueType>(1.0);
      at(2,2) = static_cast<ValueType>(1.0);
      at(3,3) = static_cast<ValueType>(1.0);
    }

    void translate(ValueType x, ValueType y, ValueType z) {
      MatrixT tmp;
      tmp.m[12] = x;
      tmp.m[13] = y;
      tmp.m[14] = z;
      mult(tmp);
    }

    void scale(const Vector3D& v) {
      MatrixT tmp;
      tmp.m[0] = v.x;
      tmp.m[5] = v.y;
      tmp.m[10] = v.z;
      mult(tmp);
    }

    void translate(const Vector3DT<ValueType>& v) {
      translate(v.x,v.y,v.z);
    }

    const MatrixT& operator *= (const MatrixT& d) {
      return mult(d);
    }

    MatrixT& mult(const MatrixT& d) {
      MatrixT tmp;

      tmp.m[ 0] = m[0] * d.m[ 0] + m[4] * d.m[ 1] + m[ 8] * d.m[ 2] + m[12] * d.m[ 3];
      tmp.m[ 4] = m[0] * d.m[ 4] + m[4] * d.m[ 5] + m[ 8] * d.m[ 6] + m[12] * d.m[ 7];
      tmp.m[ 8] = m[0] * d.m[ 8] + m[4] * d.m[ 9] + m[ 8] * d.m[10] + m[12] * d.m[11];
      tmp.m[12] = m[0] * d.m[12] + m[4] * d.m[13] + m[ 8] * d.m[14] + m[12] * d.m[15];

      tmp.m[ 1] = m[1] * d.m[ 0] + m[5] * d.m[ 1] + m[ 9] * d.m[ 2] + m[13] * d.m[ 3];
      tmp.m[ 5] = m[1] * d.m[ 4] + m[5] * d.m[ 5] + m[ 9] * d.m[ 6] + m[13] * d.m[ 7];
      tmp.m[ 9] = m[1] * d.m[ 8] + m[5] * d.m[ 9] + m[ 9] * d.m[10] + m[13] * d.m[11];
      tmp.m[13] = m[1] * d.m[12] + m[5] * d.m[13] + m[ 9] * d.m[14] + m[13] * d.m[15];

      tmp.m[ 2] = m[2] * d.m[ 0] + m[6] * d.m[ 1] + m[10] * d.m[ 2] + m[14] * d.m[ 3];
      tmp.m[ 6] = m[2] * d.m[ 4] + m[6] * d.m[ 5] + m[10] * d.m[ 6] + m[14] * d.m[ 7];
      tmp.m[10] = m[2] * d.m[ 8] + m[6] * d.m[ 9] + m[10] * d.m[10] + m[14] * d.m[11];
      tmp.m[14] = m[2] * d.m[12] + m[6] * d.m[13] + m[10] * d.m[14] + m[14] * d.m[15];

      tmp.m[ 3] = m[3] * d.m[ 0] + m[7] * d.m[ 1] + m[11] * d.m[ 2] + m[15] * d.m[ 3];
      tmp.m[ 7] = m[3] * d.m[ 4] + m[7] * d.m[ 5] + m[11] * d.m[ 6] + m[15] * d.m[ 7];
      tmp.m[11] = m[3] * d.m[ 8] + m[7] * d.m[ 9] + m[11] * d.m[10] + m[15] * d.m[11];
      tmp.m[15] = m[3] * d.m[12] + m[7] * d.m[13] + m[11] * d.m[14] + m[15] * d.m[15];

      *this = tmp;
      return *this;
    }

    // Vector等の配列に対してmultを行う
    // マルチコア,GPUの使用などを考慮したい
    template <typename VT, size_t N>
    void multArray(VT(& list)[N]) {
      BOOST_FOREACH( VT& v, list) {
        v = mult(v);
      }
    }
    template <typename ContainerT>
    void multArray(ContainerT& list) {
      BOOST_FOREACH(typename ContainerT::value_type& v, list) {
        v = mult(v);
      }
    }
    template <typename ST>
    void mult2dArray(ST& list) {
      BOOST_FOREACH(typename ST::value_type& v, list) {
        v = mult2d(v);
      }
    }

    // rotateのヘルパ
    void rotateX(const ValueType& ang) { rotate(ang, Vector3DT<ValueType>(1.0f,0.0f,0.0f)); }
    void rotateY(const ValueType& ang) { rotate(ang, Vector3DT<ValueType>(0.0f,1.0f,0.0f)); }
    void rotateZ(const ValueType& ang) { rotate(ang, Vector3DT<ValueType>(0.0f,0.0f,1.0f)); }
    void rotateXrad(const ValueType& ang) { rotate(ang, Vector3DT<ValueType>(1.0f,0.0f,0.0f),Radian); }
    void rotateYrad(const ValueType& ang) { rotate(ang, Vector3DT<ValueType>(0.0f,1.0f,0.0f),Radian); }
    void rotateZrad(const ValueType& ang) { rotate(ang, Vector3DT<ValueType>(0.0f,0.0f,1.0f),Radian); }

    // rotate glRotateと同じ
    // vector axisを軸にangだけ回転
    void rotate(const ValueType& ang, const Vector3DT<ValueType>& axis, AngleType angt=Angle) {
	  MatrixT tmp;
	  ValueType	a, si, co, ico;
	  ValueType	xi, yi, zi, mag;
	  ValueType	xx, yy, zz, xy, yz, zx, xs, ys, zs;
  
      if (angt==Radian) { 
        a = static_cast<ValueType>(ang);
      }
      else {
        a = static_cast<ValueType>(ang * Pai2Rad);
      }
      si = sin(a);
      co = cos(a);

      // Ｘ、Ｙ、Ｚ軸のみの場合最適化。
      if (axis.y == 0.0 && axis.z == 0.0) {
        tmp.m[0] =  1.0; tmp.m[4] =  0.0; tmp.m[ 8] =  0.0; tmp.m[12] = 0.0;
        tmp.m[1] =  0.0; tmp.m[5] =  co;  tmp.m[ 9] = -si;  tmp.m[13] = 0.0;
        tmp.m[2] =  0.0; tmp.m[6] =  si;  tmp.m[10] =  co;  tmp.m[14] = 0.0;
        tmp.m[3] =  0.0; tmp.m[7] =  0.0; tmp.m[11] =  0.0; tmp.m[15] = 1.0;
      }
      else if (axis.x == 0.0 && axis.z == 0.0) {
        tmp.m[0] =  co;  tmp.m[4] =  0.0; tmp.m[ 8] =  si;  tmp.m[12] = 0.0;
        tmp.m[1] =  0.0; tmp.m[5] =  1.0; tmp.m[ 9] =  0.0; tmp.m[13] = 0.0;
        tmp.m[2] = -si;  tmp.m[6] =  0.0; tmp.m[10] =  co;  tmp.m[14] = 0.0;
        tmp.m[3] =  0.0; tmp.m[7] =  0.0; tmp.m[11] =  0.0; tmp.m[15] = 1.0;
      }
      else if (axis.x == 0.0 && axis.y == 0.0) {
        tmp.m[0] =  co;  tmp.m[4] = -si;  tmp.m[ 8] =  0.0; tmp.m[12] = 0.0;
        tmp.m[1] =  si;  tmp.m[5] =  co;  tmp.m[ 9] =  0.0; tmp.m[13] = 0.0;
        tmp.m[2] =  0.0; tmp.m[6] =  0.0; tmp.m[10] =  1.0; tmp.m[14] = 0.0;
        tmp.m[3] =  0.0; tmp.m[7] =  0.0; tmp.m[11] =  0.0; tmp.m[15] = 1.0;
      }
      else {  //  最適化されない場合。
        mag = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        if (mag <= 1.0e-4) {
          return;
        }

        xi = axis.x / mag;
        yi = axis.y / mag;
        zi = axis.z / mag;

        xx = xi * xi;	yy = yi * yi;	zz = zi * zi;
        xy = xi * yi;	yz = yi * zi;	zx = zi * xi;
        xs = xi * si;	ys = yi * si;	zs = zi * si;

        ico = static_cast<ValueType>(1.0 - co);

        tmp.m[0] = (ico * xx) + co;	tmp.m[4] = (ico * xy) - zs;	tmp.m[ 8] = (ico * zx) + ys; tmp.m[12] = 0.0;
        tmp.m[1] = (ico * xy) + zs;	tmp.m[5] = (ico * yy) + co;	tmp.m[ 9] = (ico * yz) - xs; tmp.m[13] = 0.0;
        tmp.m[2] = (ico * zx) - ys;	tmp.m[6] = (ico * yz) + xs;	tmp.m[10] = (ico * zz) + co; tmp.m[14] = 0.0;
        tmp.m[3] =  0.0; 			tmp.m[7] =  0.0; 			tmp.m[11] =  0.0; 			 tmp.m[15] = 1.0;
      }
      mult(tmp);
    }

    // vを2D系の座標に変換する(normalizeするだけ)
    Vector3DT<ValueType> mult2d(const Vector3DT<ValueType>& v) const {
      Vector3DT<ValueType> dst;
      dst.x = v.x * m[0] + v.y * m[4] + v.z * m[ 8] + m[12];
      dst.y = v.x * m[1] + v.y * m[5] + v.z * m[ 9] + m[13];
      dst.z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
      ValueType iw = static_cast<ValueType>(1.0) / (v.x * m[3] + v.y * m[7] + v.z * m[11] +  m[15]);
      dst.x *= iw;
      dst.y *= iw;
     
      return dst;
    }

    Vector3DT<ValueType> mult(const Vector3DT<ValueType>& v) const {
      Vector3DT<ValueType> dst;
      dst.x = v.x * m[0] + v.y * m[4] + v.z * m[ 8] + m[12];
      dst.y = v.x * m[1] + v.y * m[5] + v.z * m[ 9] + m[13];
      dst.z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
      return dst;
    }

    // OpenGLのglLookAtと同じ
    void lookAt(const Vector3DT<ValueType>& eye, const Vector3DT<ValueType>& center, const Vector3DT<ValueType>& up) {
      MatrixT<ValueType> m;
      Vector3DT<ValueType> x, y, z;
      ValueType mag;
      /* Make rotation matrix */

      /* Z vector */
      z = eye - center;
      mag = z.length();
      if(mag > static_cast<ValueType>(0.0)) {
        z /= mag;
      }

      /* Y vector */
      y = up;

      /* X vector = Y cross Z */
      x.x =  y.y * z.z - y.z * z.y;
      x.y = -y.x * z.z + y.z * z.x;
      x.z =  y.x * z.y - y.y * z.x;

      /* Recompute Y = Z cross X */
      y.x =  z.y * x.z - z.z * x.y;
      y.y = -z.x * x.z + z.z * x.x;
      y.z =  z.x * x.y - z.y * x.x;

      /* cross product gives area of parallelogram, which is < 1.0 for
       * non-perpendicular unit-length vectors; so normalize x, y here
       */
      mag = x.length();
      if(mag > static_cast<ValueType>(0.0)) {
        x /= mag;
      }

      mag = y.length();
      if(mag > static_cast<ValueType>(0.0)) {
        y /= mag;
      }

      m.m[ 0] = x.x;
      m.m[ 4] = x.y;
      m.m[ 8] = x.z;
      m.m[12] = 0.0;

      m.m[1+ 0] = y.x;
      m.m[1+ 4] = y.y;
      m.m[1+ 8] = y.z;
      m.m[1+12] = 0.0;

      m.m[2+ 0] = z.x;
      m.m[2+ 4] = z.y;
      m.m[2+ 8] = z.z;
      m.m[2+12] = 0.0;

      m.m[3+ 0] = 0.0;
      m.m[3+ 4] = 0.0;
      m.m[3+ 8] = 0.0;
      m.m[3+12] = 1.0;

      *this *= m;

      /* Translate Eye to Origin */
      this->translate(Vector3DT<ValueType>(-eye.x, -eye.y, -eye.z));
    }

    void frustum(const ValueType& left,
                 const ValueType& right,
                 const ValueType& bottom,
                 const ValueType& top,
                 const ValueType& nearval,
                 const ValueType& farval) {
      ValueType x, y, a, b, c, d;
      MatrixT<ValueType> m;
      ValueType two = static_cast<ValueType>(2.0);

      x = (two * nearval) / (right - left);
      y = (two * nearval) / (top - bottom);
      a = (right + left) / (right - left);
      b = (top + bottom) / (top - bottom);
      c = -(farval + nearval) / ( farval - nearval);
      d = -(two * farval * nearval) / (farval - nearval);

      m.m[0+ 0] = x;   
      m.m[0+ 4] = 0.0; 
      m.m[0+ 8] = a;   
      m.m[0 + 12] = 0.0;

      m.m[1+ 0] = 0.0; 
      m.m[1+ 4] = y;
      m.m[1+ 8] = b;
      m.m[1 + 12] = 0.0;

      m.m[2+ 0] = 0.0;
      m.m[2+ 4] = 0.0;
      m.m[2+ 8] = c;
      m.m[2 + 12] = d;


      m.m[3+ 0] = 0.0;
      m.m[3+ 4] = 0.0;
      m.m[3+ 8] = static_cast<ValueType>(-1.0);
      m.m[3 + 12] = 0.0;

      *this *= m;
    }

    // gluPerspectiveとおなじ
    void perspective(const ValueType fovy, const ValueType aspect, const ValueType zNear, const ValueType zFar) {
      ValueType xmin, xmax, ymin, ymax;
      ymax =  zNear * tan(static_cast<ValueType>(fovy * 3.14159265358979 / 360.0));
      ymin = -ymax;
      xmin =  ymin * aspect;
      xmax =  ymax * aspect;

      frustum(xmin, xmax, ymin, ymax, zNear, zFar);
    }

    //gluUnProject
    bool unProject(ValueType winx, ValueType winy, ValueType winz,
            	     const MatrixT& viewMatrix,
	                 const RectS32& viewport,
            	     Vector3DT<ValueType>& obj) {
      MatrixT mM;
      MatrixT mA;
      ValueType in[4], out[4];

      in[0] = static_cast<ValueType>((winx - viewport.left) * 2.0 / viewport.width - 1.0);
      in[1] = static_cast<ValueType>((winy - viewport.top) * 2.0 / viewport.height - 1.0);
      in[2] = static_cast<ValueType>(2.0 * winz - 1.0);
      in[3] = 1.0f;

      invert_matrix(viewMatrix, *this);

      transform_point(out, *this, in);
      if (out[3] == 0.0) return false; 

      obj.x = out[0] / out[3];
      obj.y = out[1] / out[3];
      obj.z = out[2] / out[3];
      return true;
    }

    // row majorとcolumn majorを入れ替える
    //  0   4   8   12        0   1   2   3
    //  1   5   9   13        4   5   6   7
    //  2   6   10  14        8   9  10  11
    //  3   7   11  15       12  13  14  15 
    void majorConvert() {
      swap(m[4], m[1]);
      swap(m[8], m[2]);
      swap(m[12], m[3]);
      swap(m[9],  m[6]);
      swap(m[13], m[7]);
      swap(m[14], m[11]);
    }

  private:
    static bool invert_matrix(const MatrixT& m, MatrixT& out);
    static void transform_point(ValueType out[4], const MatrixT& m, const ValueType in[4]);


  };

  template <typename Vt>
  MatrixT<Vt> operator * (const MatrixT<Vt>& s, const MatrixT<Vt>& d) {
    MatrixT<Vt> tmp(s);
    return tmp.mult(d);
  }



  typedef MatrixT<float32_t> Matrix;
  typedef MatrixT<float64_t> Matrixd;



}
// テンプレートはビルドされないのでインクルード？
//#include "Matrix.cpp"
#endif
