/** Vector.h -*-C++-*-





*/


#ifndef TS_Vector_h
#define TS_Vector_h

#include <memory.h>
#include <vector>
#include "libTS/Common/Common.h"
#include "libTS/Util/Logger.h"
#include "Math.hpp"

namespace ts {

  template <typename T>
  struct Vector3DT {
    enum { Elements = 3 };
    typedef T value_type;
    T x;
    T y;
    T z;
    Vector3DT() : x(0), y(0), z(0) {}
    Vector3DT(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    Vector3DT(const T (&ar)[Elements]) : x(ar[0]), y(ar[1]), z(ar[2]) {}
    
    Vector3DT(const Vector3DT<float32_t>& dst) 
      : x(static_cast<T>(dst.x))
      , y(static_cast<T>(dst.y))
      , z(static_cast<T>(dst.z))
    {}
    Vector3DT(const Vector3DT<float64_t>& dst) 
      : x(static_cast<T>(dst.x))
      , y(static_cast<T>(dst.y))
      , z(static_cast<T>(dst.z))
    {}
    Vector3DT& operator = (const Vector3DT<float32_t>& dst) {
      x = static_cast<T>(dst.x);
      y = static_cast<T>(dst.y);
      z = static_cast<T>(dst.z);
      return *this;
    }
    Vector3DT& operator = (const Vector3DT<float64_t>& dst) {
      x = static_cast<T>(dst.x);
      y = static_cast<T>(dst.y);
      z = static_cast<T>(dst.z);
      return *this;
    }

    Vector3DT& operator += (const Vector3DT& v) {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }
    Vector3DT operator + (const Vector3DT& v) {
      return Vector3DT(x + v.x, y + v.y, z + v.z);
    }
    Vector3DT& operator -= (const Vector3DT& v) {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }
    Vector3DT operator - (const Vector3DT& v) {
      return Vector3DT(x - v.x, y - v.y, z - v.z);
    }
    Vector3DT& operator *= (const T& m) {
      x *= m;
      y *= m;
      z *= m;
      return *this;
    }

    Vector3DT& operator *= (const Vector3DT& m) {
      x *= m.x;
      y *= m.y;
      z *= m.z;
      return *this;
    }

    Vector3DT& operator /= (const T& m) {
      x /= m;
      y /= m;
      z /= m;
      return *this;
    }

    Vector3DT& operator /= (const Vector3DT& m) {
      x /= m.x;
      y /= m.y;
      z /= m.z;
      return *this;
    }


    Vector3DT operator * (const T& m) {
      return Vector3DT(x * m, y * m, z * m);
    }

    template <typename VT>
    Vector3DT operator * (const Vector3DT<VT>& m) {
      return Vector3DT(x * m.x, y * m.y, z * m.z);
    }

    Vector3DT operator / (const T& m) {
      return Vector3DT(x / m, y / m, z / m);
    }

    Vector3DT operator / (const Vector3DT& m) {
      return Vector3DT(x / m.x, y / m.y, z / m.z);
    }

    Vector3DT operator - () {
      return Vector3DT(-x, -y, -z);
    }

    void clear() {
      x = y = z = 0.0;
    }

    bool isZero() const {
      return x == 0.0 && y == 0.0 && z == 0.0;
    }
    
    T length() const {
      return sqrt(x * x + y * y + z * z);
    }

    T distance(const Vector3DT& dst) const {
      return sqrt((x-dst.x)*(x-dst.x) + (y-dst.y)*(y-dst.y) + (z-dst.z)*(z-dst.z));
    }

    Vector3DT& normalize() {
      T w = static_cast<T>(1.0) / sqrt(x*x + y*y + z*z);
      x *= w;
      y *= w;
      z *= w;
      return *this;
    }
    
    // vとの外積を求める!
    Vector3DT cross(const Vector3DT& v) const {
      return Vector3DT((y*v.z)-(z*v.y), (z*v.x)-(x*v.z), (x*v.y)-(y*v.x));
    }

    // vとの内積を求める!
    T dot(const Vector3DT& v) const {
      return (x * v.x) + (y * v.y) + (z * v.z);
    }


    bool operator == (const Vector3DT& d) const {
      return x == d.x && y == d.y && z == d.z;
    }

    static void CopyArray(Vector3DT* dst, const Vector3DT* src, uint32_t size) {
      memcpy(dst, src, sizeof(Vector3DT)*size);
    }

    // 直線v1-v2と直線v3-v4のxy平面上における交点を求める!
    static Vector3DT getCrossPoint2D(const Vector3DT& v1, const Vector3DT& v2, const Vector3DT& v3, const Vector3DT& v4) {
      // v1-v2の直線の方程式 ax+by+c = 0;
      T a = v2.y - v1.y;
      T b = v1.x - v2.x;
      T c = -a * v1.x - b * v1.y;
      // v3-v4の直線の方程式 gx+hy+i = 0;
      T g = v4.y - v3.y;
      T h = v3.x - v4.x;
      T i = -g * v3.x - h * v3.y;
      // 2つの直線の交点を求める!
      T cross = -b*g + h*a;
      cross = -cross;
      if (cross != 0.0) {
        return Vector3DT((-b*i + c*h)/cross , (i*a - c*g)/cross, static_cast<T>((v1.z+v2.z+v3.z+v4.z)/4.0));
      }
      else {
        //２つの直線は平行!
        return Vector3DT(0.0,0.0,0.0);
      }
    }

    // 直線v1-v2 の直線上のzで示される位置をセットする!
    Vector3DT& calcPointZonLine(const Vector3DT& v1, const Vector3DT& v2, T vz) {
      // 直線の方程式 (x-v1.x)/(v2.x-v1.x)=(y-v1.y)/(v2.y-v1.y)=(z-v1.z)/(v2.z-v1.z)
      T k = (vz-v1.z)/(v2.z-v1.z);
      this->x = k * (v2.x - v1.x) + v1.x; 
      this->y = k * (v2.y - v1.y) + v1.y; 
      this->z = vz;
      return *this;
    }

    // 正規化されているベクタのX軸(仰角), Z軸(方角)の回転角度を計算する!
    void calcRotateXZ(T& rx, T& rz) {
      // 仰角の計算 (-90度〜90度まで)!
      rx = asin(z); 
      // XY平面での半径を計算!
      T nx = x;
      T k = 1.0 / sqrt(x*x+y*y);
      nx *= k;
      rz = asin(nx); // 回転角度!
      if(y < 0.0) rz = static_cast<T>(math::PI) - rz;
      if(rz < 0.0f) rz += static_cast<T>(math::PI2);
    }


    // 3点からなる平面の法線ベクトルの計算してセットする!
    Vector3DT& calcNormals(const Vector3DT& v1, const Vector3DT& v2, const Vector3DT& v3) {
      //void gCalcNormals(float &nx, float &ny, float &nz,
	  //	 float x1, float y1, float z1,
	  //	 float x2, float y2, float z2,
	  //	 float x3, float y3, float z3)
      // nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
      // ny = x1*(z2-z3) + x2*(z3-z1) + x3*(z1-z2);
      // nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
      x = v1.y*(v2.z-v3.z) + v2.y*(v3.z-v1.z) + v3.y*(v1.z-v2.z);
      y = v1.x*(v2.z-v3.z) + v2.x*(v3.z-v1.z) + v3.x*(v1.z-v2.z);
      z = v1.x*(v2.y-v3.y) + v2.x*(v3.y-v1.y) + v3.x*(v1.y-v2.y);
    
      //(k*nx)^2 +    (k*ny)^2 +    (k*nz)^2  = 1;
      //k*nx*k*nx + k*ny*k*ny + k*nz*k*nz = 1;
      //k2*nx2 + k2+ny2 + k2+kz2 = 1;
      //k2(nx2+ny2+nz2) = 1;
      //k2 = 1/ (nx2+ny2+nz2);
      T k = static_cast<T>(1.0) / sqrt(x*x+y*y+z*z);
      x *= -k;
      y *= -k;
      z *= -k;
      return *this;
    }


  };

  template <typename FloatT>
  Vector3DT<FloatT> operator + (const Vector3DT<FloatT>& v1, const Vector3DT<FloatT>&v2) {
    Vector3DT<FloatT> v(v1);
    v += v2;
    return v;
  }

  template <typename FloatT>
  Vector3DT<FloatT> operator - (const Vector3DT<FloatT>& v1, const Vector3DT<FloatT>&v2) {
    Vector3DT<FloatT> v(v1);
    v -= v2;
    return v;
  }

  template <typename FloatT>
  Vector3DT<FloatT> operator / (FloatT v1, const Vector3DT<FloatT>&v2) {
    return Vector3DT<FloatT>(v1/v2.x, v1/v2.y, v1/v2.z);
  }
  template <typename FloatT>
  Vector3DT<FloatT> operator * (FloatT v1, const Vector3DT<FloatT>&v2) {
    return Vector3DT<FloatT>(v1*v2.x, v1*v2.y, v1*v2.z);
  }

  typedef Vector3DT<int32_t> Vector3Di;
  typedef Vector3DT<float32_t> Vector3D;
  typedef Vector3DT<float64_t> Vector3Dd;

  // Vector2D
  // 二次元系のベクタ!
  template <typename FloatT>
  struct Vector2DT {
    enum { Elements = 2 };
    typedef FloatT value_type;
    Vector2DT() {}
    template <typename T>
    Vector2DT(T x_, T y_)
      : x(static_cast<FloatT>(x_)), y(static_cast<FloatT>(y_)) {}

    Vector2DT(FloatT x_, FloatT y_)
      : x(x_), y(y_) {}

    template <typename T>
    Vector2DT(const Vector2DT<T>& v)
      : x(static_cast<FloatT>(v.x)), y(static_cast<FloatT>(v.y)) {}

    Vector2DT(const Vector3DT<FloatT>& v3)
      : x(v3.x), y(v3.y) {}


    Vector2DT& operator += (const Vector2DT& v) {
      x += v.x;
      y += v.y;
      return *this;
    }
    Vector2DT& operator -= (const Vector2DT& v) {
      x -= v.x;
      y -= v.y;
      return *this;
    }
    Vector2DT& operator *= (FloatT m) {
      x *= m;
      y *= m;
      return *this;
    }
    Vector2DT& operator /= (FloatT m) {
      x /= m;
      y /= m;
      return *this;
    }

    Vector2DT operator + (const Vector2DT& v) {
      return Vector2D(x+v.x, y+v.y);
    }
    Vector2DT operator - (const Vector2DT& v) {
      return Vector2D(x-v.x, y-v.y);
    }
    Vector2DT operator * (const Vector2DT& v) {
      return Vector2D(x*v.x, y*v.y);
    }
    Vector2DT operator / (const Vector2DT& v) {
      return Vector2D(x/v.x, y/v.y);
    }
    Vector2DT operator * (FloatT v) {
      return Vector2D(x*v, y*v);
    }
    Vector2DT operator / (FloatT v) {
      return Vector2D(x/v, y/v);
    }

    Vector3DT<FloatT> to3D(FloatT z) {
      return Vector3DT<FloatT>(x, y, z);
    }


    void swap(const Vector2DT& v) {
      swap(x, v.x);
      swap(y, v.y);
    }

    // 回転 (単位は度)!
    void rotate(FloatT th, const Vector2DT& origin = Vector2DT(0.0f,0.0f)) {
      th *=  static_cast<FloatT>(Pai2Rad);
      FloatT sr = sin(th);
      FloatT cr = cos(th);
      FloatT vx = x - origin.x;
      FloatT vy = y - origin.y;
      x = (vx * cr - vy * sr) + origin.x;
      y = (vx * sr + vy * cr) + origin.y;
    }

    Vector2DT& normalize() {
      FloatT w = sqrt(x*x + y*y);
      if (w != 0.0) {
        x /= w;
        y /= w;
      }
      else {
        x = y = 0.0;
      }
      return *this;
    }
 
    // vとの外積を求める!
    FloatT cross(const Vector2DT& v) const {
      return (x * v.y) - (v.x * y);
    }

    // vとの内積を求める!
    FloatT dot(const Vector2DT& v) const {
      return x*v.x + y*v.y;
    }

    FloatT distance(const Vector2DT& v) const {
      return sqrt((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y));
    }

    FloatT length() const {
      return sqrt(x*x + y*y);
    }

    FloatT x;
    FloatT y;
  };


  typedef Vector2DT<float32_t> Vector2D;
  typedef Vector2DT<float64_t> Vector2Dd;
  typedef Vector2DT<int32_t>   Vector2Di;




  template <typename FloatT>
  Vector2DT<FloatT> operator - (const Vector2DT<FloatT>& a, const Vector2DT<FloatT>& b) {
    return Vector2DT<FloatT>(a.x - b.x, a.y - b.y);
  }
  template <typename FloatT>
  Vector2DT<FloatT> operator + (const Vector2DT<FloatT>& a, const Vector2DT<FloatT>& b) {
    return Vector2DT<FloatT>(a.x + b.x, a.y + b.y);
  }




}

#endif

