// Geometric.hpp

#ifndef TS_GEOMETRIC_HPP
#define TS_GEOMETRIC_HPP
#ifdef WIN32
#pragma warning (push)
#pragma warning (disable : 4100)
#endif

#include <boost/concept_check.hpp>
#include <boost/foreach.hpp>
#include <limits>

#include "Math.hpp"
#include "Matrix.h"
#include "Vector.h"
//#include "Vertices.h"
#include "Rect.h"

namespace ts {
namespace math {


  // �O�p�`�̖ʐς����߂�
  // �w�����̌���:
  //  3�ӂ̒����� a, b, c
  //    s = (a + b + c) / 2;
  //  �Ƃ���Ƃ��A�ʐ� S ��
  //    S = sqrt(s * (s - a) * (s - b) * (s - c) )
  //  �ŋ��߂���B
  // 
  //  TriangleArea2�ł́Aa,b,c���R�ӂƂ���O�p�`�̖ʐς̓���Ԃ�
  //
  template <typename FT>
  inline FT TriangleArea2(FT a, FT b, FT c) {
    FT s = (a + b + c) / 2;
    return s * (s - a) * (s - b) * (s - c);
  }


  // @brief ����(l1-l2)�ƕ���(p1-p2-p3)�̌�_�����߂�
  //
  // @param[in]  l1,l2      �����̒��_
  // @param[in]  p1,p2,p3   ���ʂ̒��_
  // @param[in]  insideLine l1-l2�̊Ԃ�crossPoint������ꍇ�̂݌v�Z����
  // @param[out] crossPoint ��_
  // @return     true ��_�̌v�Z�ɐ���
  //
  template <typename FT>
  inline bool LinePlaneCrossPoint(
    const Vector3DT<FT>& l1, const Vector3DT<FT>& l2,
    const Vector3DT<FT>& p1, const Vector3DT<FT>& p2, const Vector3DT<FT>& p3,
    Vector3DT<FT>& crossPoint,
    bool insideLine = false) {

    // �����̃x�N�g��
    Vector3DT<FT> lv = l2 - l1;
    lv.normalize();

    // ���ʂ̖@���x�N�g��
    Vector3DT<FT> pn = (p2 - p1).cross(p3 - p1);
    pn.normalize();

    // �����ƕ��ʂ̃x�N�g��
    Vector3DT<FT> lpv1 = p2 - l1;

    // ��_�����߂�
    FT a1, a2,  b;
    a1 = lpv1.dot(pn);
    b = lv.dot(pn);
    if (b == static_cast<FT>(0.0)) {
      // ���s�Ȃ̂Ō�_�͂Ȃ�
      return false;
    }

    // crossPoint��l1-l2�ԂɂȂ������ׂ�
    if (insideLine) {
      Vector3DT<FT> lpv2 = p2 - l2;
      a2 = lpv2.dot(pn);
      if ((a1 < 0 && a2 < 0) || (a1 > 0 && a2 > 0)) {
        return false; // l1-l2�ԂɂȂ��̂Ŏ��s
      }
    }

    // �����̕������ɑ�����Č�_�����߂�
    FT t = a1 / b;
    crossPoint.x = l1.x + lv.x * t;
    crossPoint.y = l1.y + lv.y * t;
    crossPoint.z = l1.z + lv.z * t;

    // ��_����
    return true;
  }

  // �_d������p1-p2-p3�̂ǂ��瑤�ɂ��邩���ׂ�
  // @return -1: ���� 0: ���ʏ� 1: �\��
  enum SideType {
    BackSide = 0,
    FrontSide = 1,
    OnSurface = 2,
  };
  template <typename FT>
  inline SideType SideCheck3D(const Vector3DT<FT>& d,
                         const Vector3DT<FT>& p1, const Vector3DT<FT>& p2, const Vector3DT<FT>& p3) {

    // ���ʂ̖@���x�N�g��
    Vector3DT<FT> normal = (p2 - p1).cross(p3 - p1);
    normal.normalize();
    // �@���Ƃ̓���
    FT a = (p1 - d).dot(normal);
    if( a > 0 )      { return FrontSide; } // �\
    else if( a < 0 ) { return BackSide; } // ��
    else               { return OnSurface; } // ���ʏ�
  }


  // xy���ʏ�ł̒���v1-v2 �Ɠ_ t�̈ʒu�֌W�𕄍��ŕԂ�
  template <typename VtxT1, typename VtxT2>
  inline int SideCheck2D(const VtxT1& v1, const VtxT1& v2, const VtxT2& t) {
   // v1-v2�̒����̕����� ax+by+c = 0;
    VtxT1::value_type a = v2.y - v1.y;
    VtxT1::value_type b = v1.x - v2.x;
    VtxT1::value_type c = -a * v1.x - b * v1.y;
    VtxT1::value_type side = a * t.x + b * t.y + c;
    if (side == static_cast<VtxT1::value_type>(0.0)) return 0;
    return side < 0 ? -1 : 1;
  }


  // ���_�̏W��
  // �����ȉ��̂��̂͊ۂ߂���
  template <typename FloatT>
  struct Vertex3DSetT {
    Vertex3DSetT() : minRange(static_cast<FloatT>(0.001)) 
    {}

    void insert(const Vector3DT<FloatT>& newVec) {
      BOOST_FOREACH(Vector3DT<FloatT>& v, vertexList) {
        if (fabs(v.x - newVec.x) < minRange
          && fabs(v.y - newVec.y) < minRange
          && fabs(v.z - newVec.z) < minRange) {
            return;
        }
      }
      vertexList.push_back(newVec);
    }

	struct Compare {
      Vector3DT<FloatT> center;
      Vector3DT<FloatT> firstVec;
      Vector3DT<FloatT> sideNormal;
      Compare(const Vector3DT<FloatT>& c, 
		      const Vector3DT<FloatT>& fv,
		      const Vector3DT<FloatT>& sn)
		      : center(c), firstVec(fv), sideNormal(sn) {}
      bool operator()(const Vector3DT<FloatT>& a, const Vector3DT<FloatT>& b) const {
        FloatT ath = firstVec.dot((a - center).normalize());
        FloatT bth = firstVec.dot((b - center).normalize());
        if (sideNormal.dot(a - center) > 0) {
	      ath = static_cast<FloatT>(2.0 - ath);
        }
        if (sideNormal.dot(b - center) > 0) {
	      bth = static_cast<FloatT>(2.0 - bth);
        }

        return ath < bth;
      }
	};

	  // ���ꕽ�ʏ�ɂ���_�𔽎��v���ɕ��ѕς���
    // ���S���W�Ƃ̓���(�Ȃ��p)�̏��Ń\�[�g����
    // verticesNormal ���ʂ̖@���x�N�g��
    void aroundSort(const Vector3DT<FloatT>& verticesNormal) {
      // ���S�_�����߂�
      center.clear();
      BOOST_FOREACH(Vector3DT<FloatT>& v, vertexList) {
        center += v;
      }
      center /= static_cast<FloatT>(vertexList.size());
      DEBUGLOG("center=%f,%f,%f\n", center.x, center.y, center.z);

      DEBUGLOG("v0=%f,%f,%f\n", vertexList[0].x, vertexList[0].y, vertexList[0].z);
      Vector3DT<FloatT> basePoint = vertexList[0] - center; // ��ƂȂ�ŏ��̓_

      // ���E�ƂȂ镽�ʂ̖@�������߂�
      Vector3DT<FloatT> sideNormal = basePoint.cross(verticesNormal);
      basePoint.normalize();


      std::sort(vertexList.begin(), vertexList.end(), Compare(center, basePoint, sideNormal));

      BOOST_FOREACH(Vector3DT<FloatT>& v, vertexList) {
        DEBUGLOG("%f,%f,%f dot=%f\n", v.x, v.y, v.z, basePoint.dot((v - center).normalize()));
      }


    }

    // TRIANGLE_FAN�p�̒��_���X�g�𐶐�����
    template <typename Container>
    void makeTriangleFan(Container& container, bool invert = false) {
      // TODO: ���_���X�g�̍œK�����s������
      std::back_insert_iterator<Container> ite(container);
      ite = center;
      if (invert) {
        copy (vertexList.rbegin(), vertexList.rend(), ite);
        ite = *vertexList.rbegin();
      }
      else {
        copy (vertexList.begin(), vertexList.end(), ite);
        ite = *vertexList.begin();
      }
    }


    std::vector<Vector3DT<FloatT> > vertexList;
    Vector3DT<FloatT> center;
    FloatT minRange;
  };

  typedef Vertex3DSetT<float32_t> Vertex3DSet;



    // a1,a2��[�_�Ƃ��������b1,b2��[�_�Ƃ�������̌�������
    template <typename P>
    inline bool IsIntersectedLine2D(const P& a1, const P& a2, const P& b1, const P& b2) {
      return (a2-a1).cross(b1-a1) * (a2-a1).cross(b2-a1) < 1.0e-5f
          && (b2-b1).cross(a1-b1) * (b2-b1).cross(a2-b1) < 1.0e-5f;
    }

    // a1,a2��[�_�Ƃ��������rect�̎l�ӂƂ̌�������
    template <typename P, typename R>
    inline bool IsIntersectedLine2D(const P& a1, const P& a2, const R& rect) {
        return IsIntersectedLine2D(a1, a2, P(rect.left, rect.top), P(rect.Right(), rect.top))
            || IsIntersectedLine2D(a1, a2, P(rect.left, rect.Bottom()), P(rect.Right(), rect.Bottom()))
            || IsIntersectedLine2D(a1, a2, P(rect.left, rect.top), P(rect.left, rect.Bottom()))
            || IsIntersectedLine2D(a1, a2, P(rect.Right(), rect.top), P(rect.Right(), rect.Bottom()));
    }

    // a1,a2��[�_�Ƃ��������b1,b2��[�_�Ƃ�������̌�_�����߂�
    template <typename P>
    inline P ClacIntersectPoint2D(const P& a1, const P& a2, const P& b1, const P& b2) {
        P b = b2-b1;
        P::ValueType d1 = abs(b.cross(a1-b1));
        P::ValueType d2 = abs(b.cross(a2-b1));
        P::ValueType t = d1 / (d1 + d2);
        return a1 + ((a2-a1) * t);
    }
    // a1,a2��[�_�Ƃ��������rect�Ƃ̌�_�����߂�
    template <typename P, typename R>
    inline P ClacIntersectPoint2D(const P& a1, const P& a2, const R& rect) {
        if (IsIntersectedLine2D(a1, a2, P(rect.left, rect.top), P(rect.Right(), rect.top))) {
            return ClacIntersectPoint2D(a1, a2, P(rect.left, rect.top), P(rect.Right(), rect.top));
        }
        if (IsIntersectedLine2D(a1, a2, P(rect.left, rect.Bottom()), P(rect.Right(), rect.Bottom()))) {
            return ClacIntersectPoint2D(a1, a2, P(rect.left, rect.Bottom()), P(rect.Right(), rect.Bottom()));
        }
        if (IsIntersectedLine2D(a1, a2, P(rect.left, rect.top), P(rect.left, rect.Bottom()))) {
            return ClacIntersectPoint2D(a1, a2, P(rect.left, rect.top), P(rect.left, rect.Bottom()));
        }
        if (IsIntersectedLine2D(a1, a2, P(rect.Right(), rect.top), P(rect.Right(), rect.Bottom()))) {
            return ClacIntersectPoint2D(a1, a2, P(rect.Right(), rect.top), P(rect.Right(), rect.Bottom()));
        }
        return rect.Center();
    }





}} // namespace ts::math

#pragma warning (pop)
#endif
