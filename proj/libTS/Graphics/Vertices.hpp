#ifndef TS_Vertices_hpp
#define TS_Vertices_hpp

#ifndef STDAFX_H
#include <memory.h>
#include <vector>
#endif
#include "libTS/math/vec.hpp"
#include "libTS/math/rect.hpp"
#include "libTS/Image/Color.hpp"

namespace ts {


   // �e�N�X�`�����W
  // 0�ŏ����������
  struct TexCoords {
    enum { Elements = 2 };
    typedef float32_t value_type;
    float32_t u;
    float32_t v;
    TexCoords(float32_t u_ = 0.0f, float32_t v_ = 0.0f)
      : u(u_), v(v_)
    {}
    TexCoords(const float32_t (&ar)[2]) : u(ar[0]), v(ar[1]) {}
  };

  // ���_�f�[�^�̃^�C�v
  enum VertexType {
    VertexType_XYZ,       // xyz�̒��_�̂�
    VertexType_XYZUV,     // ���_+UV
    VertexType_XYZUVN,    // ���_+UV+Normal
    VertexType_XYZN,      // ���_+Normal
    VertexType_XYZC,       // ���_+Color
    VertexType_D3DVERTEX  // D#DVERTEX�݊� x,y,z,nx,ny,nz,tu,tv
  };

  // ���_���
  template <typename FloatT>
  struct VertexT : Vector3DT<FloatT> {
    enum { Elements = 3 };
    enum { type = VertexType_XYZ };
  };

  // �@���t�����_
  template <typename FloatT>
  struct VertexWithNormalT : VertexT<FloatT> { 
    enum { Elements = 6};
    enum { type = VertexType_XYZN };
    Vector3DT<FloatT> normal;
  };
  // �e�N�X�`���t�����_
  template <typename FloatT>
  struct VertexWithTexCoordsT : VertexT<FloatT> {
    enum { type = VertexType_XYZUV };
    enum { Elements = 5 };
    TexCoords texcoords;
  };
  // �@���e�N�X�`���t�����_
  template <typename FloatT>
  struct VertexWithNormalTexCoordsT  : VertexT<FloatT> {
    enum { type = VertexType_XYZUVN };
    enum {Elements = 8};
    Vector3DT<FloatT> normal;
    TexCoords texcoords;
  };

  // �F���̒��_
  template <typename FloatT>
  struct VertexWithColorT  : VertexT<FloatT> {
    enum { type = VertexType_XYZC };
    ColorRGBA color;
  };

  typedef VertexT<float> Vertex;
  typedef VertexWithNormalT<float> VertexWithNormal;
  typedef VertexWithTexCoordsT<float> VertexWithTexCoords;
  typedef VertexWithNormalTexCoordsT<float> VertexWithNormalTexCoords;
  typedef VertexWithColorT<float> VertexWithColor;





// N�p�`�̒�`
  template <uint32_t N, typename VectorT>
  struct VerticesNT {
    VectorT vtx[N];
    typedef typename VectorT* iterator;
    typedef typename const VectorT* const_iterator;
    typedef VectorT value_type;
    VerticesNT() {}
    VerticesNT(const value_type* ptr) {
      memcpy(&vtx[0], ptr, sizeof(vtx));
    }

    iterator begin() { return &vtx[0]; }
    iterator end() { return &vtx[N]; }
    const_iterator begin() const { return &vtx[0]; }
    const_iterator end() const { return &vtx[N]; }

    VectorT& operator[] (uint32_t n) { return vtx[n]; }
    const VectorT& operator[] (uint32_t n) const { return vtx[n]; }
  };

#if 1
  typedef VerticesNT<4, Vector3D>   Vertices4_3D;
  typedef VerticesNT<4, Vector2D>   Vertices4_2D;
  typedef VerticesNT<4, Vector3Dd>  Vertices4_3Dd;
  typedef VerticesNT<4, Vector2Dd>  Vertices4_2Dd;
  typedef VerticesNT<3, Vector3D>   Vertices3_3D;
  typedef VerticesNT<3, Vector2D>   Vertices3_2D;
  typedef VerticesNT<3, Vector3Dd>  Vertices3_3Dd;
  typedef VerticesNT<3, Vector2Dd>  Vertices3_2Dd;
#else
  typedef Vector3D Vertices4_3D[4];
  typedef Vector2D Vertices4_2D[4];
  typedef Vector3Dd Vertices4_3Dd[4];
#endif
  inline void copy(Vertices4_3D& dst, const Vertices4_3D& src) {
    memcpy(&dst, &src, sizeof(dst));
  }
  inline void copy(Vertices4_2D& dst, const Vertices4_2D& src) {
    memcpy(&dst, &src, sizeof(dst));
  }
  inline void copy(Vertices4_3Dd& dst, const Vertices4_3Dd& src) {
    memcpy(&dst, &src, sizeof(dst));
  }



  template <typename FloatT>
  struct BoundaryBox {
    Vector3DT<FloatT> min;
    Vector3DT<FloatT> max;
    bool resetFlag;
    BoundaryBox() : resetFlag(false)
    {}
    void clear() { resetFlag = false; }
    void reset(const Vector3DT<FloatT>& v) {
      min.x = max.x = v.x;
      min.y = max.y = v.y;
      min.z = max.z = v.z;
      resetFlag = true;
    }
    void set(const Vector3DT<FloatT>& v) {
      if (!resetFlag) {
        reset(v);
        return;
      }
      if (min.x > v.x) min.x = v.x;
      if (min.y > v.y) min.y = v.y;
      if (min.z > v.z) min.z = v.z;
      if (max.x < v.x) max.x = v.x;
      if (max.y < v.y) max.y = v.y;
      if (max.z < v.z) max.z = v.z;
    }
    Vector3DT<FloatT> center() const {
      return Vector3DT<FloatT>((max.x + min.x)/2, (max.y + min.y)/2, (max.z + min.z)/2);
    }
    Rect<FloatT> getRect() const {
      return Rect<FloatT>(min.x, min.y, max.x - min.x, max.y - min.y);
    }
  };


  template <typename FloatT>
  struct BoundaryBox2D {
    Vector2DT<FloatT> min;
    Vector2DT<FloatT> max;
    bool resetFlag;
    BoundaryBox2D() : resetFlag(false)
    {}
    template <uint32_t N, typename VT>
    BoundaryBox2D(const VerticesNT<N,VT>& r) 
      : resetFlag(false) 
    {
      for (uint32_t n = 0; n < N; ++n) {
        set(r[n]);
      }
    }
    void clear() { resetFlag = false; }
    void reset(const Vector2DT<FloatT>& v) {
      min.x = max.x = v.x;
      min.y = max.y = v.y;
      resetFlag = true;
    }
    void set(const Vector2DT<FloatT>& v) {
      if (!resetFlag) {
        reset(v);
        return;
      }
      if (min.x > v.x) min.x = v.x;
      if (min.y > v.y) min.y = v.y;
      if (max.x < v.x) max.x = v.x;
      if (max.y < v.y) max.y = v.y;
    }
    Vector2DT<FloatT> center() const {
      return Vector2DT<FloatT>((max.x + min.x)/2, (max.y + min.y)/2);
    }
    Rect<FloatT> getRect() const {
      return Rect<FloatT>(min.x, min.y, max.x - min.x, max.y - min.y);
    }
  };

 

}

#endif
