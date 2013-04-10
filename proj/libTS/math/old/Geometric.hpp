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


  // 三角形の面積を求める
  // ヘロンの公式:
  //  3辺の長さを a, b, c
  //    s = (a + b + c) / 2;
  //  とするとき、面積 S は
  //    S = sqrt(s * (s - a) * (s - b) * (s - c) )
  //  で求められる。
  // 
  //  TriangleArea2では、a,b,cを３辺とする三角形の面積の二乗を返す
  //
  template <typename FT>
  inline FT TriangleArea2(FT a, FT b, FT c) {
    FT s = (a + b + c) / 2;
    return s * (s - a) * (s - b) * (s - c);
  }


  // @brief 直線(l1-l2)と平面(p1-p2-p3)の交点を求める
  //
  // @param[in]  l1,l2      直線の頂点
  // @param[in]  p1,p2,p3   平面の頂点
  // @param[in]  insideLine l1-l2の間にcrossPointがある場合のみ計算する
  // @param[out] crossPoint 交点
  // @return     true 交点の計算に成功
  //
  template <typename FT>
  inline bool LinePlaneCrossPoint(
    const Vector3DT<FT>& l1, const Vector3DT<FT>& l2,
    const Vector3DT<FT>& p1, const Vector3DT<FT>& p2, const Vector3DT<FT>& p3,
    Vector3DT<FT>& crossPoint,
    bool insideLine = false) {

    // 直線のベクトル
    Vector3DT<FT> lv = l2 - l1;
    lv.normalize();

    // 平面の法線ベクトル
    Vector3DT<FT> pn = (p2 - p1).cross(p3 - p1);
    pn.normalize();

    // 直線と平面のベクトル
    Vector3DT<FT> lpv1 = p2 - l1;

    // 交点を求める
    FT a1, a2,  b;
    a1 = lpv1.dot(pn);
    b = lv.dot(pn);
    if (b == static_cast<FT>(0.0)) {
      // 平行なので交点はない
      return false;
    }

    // crossPointがl1-l2間にないか調べる
    if (insideLine) {
      Vector3DT<FT> lpv2 = p2 - l2;
      a2 = lpv2.dot(pn);
      if ((a1 < 0 && a2 < 0) || (a1 > 0 && a2 > 0)) {
        return false; // l1-l2間にないので失敗
      }
    }

    // 直線の方程式に代入して交点を求める
    FT t = a1 / b;
    crossPoint.x = l1.x + lv.x * t;
    crossPoint.y = l1.y + lv.y * t;
    crossPoint.z = l1.z + lv.z * t;

    // 交点あり
    return true;
  }

  // 点dが平面p1-p2-p3のどちら側にあるか調べる
  // @return -1: 裏側 0: 平面上 1: 表側
  enum SideType {
    BackSide = 0,
    FrontSide = 1,
    OnSurface = 2,
  };
  template <typename FT>
  inline SideType SideCheck3D(const Vector3DT<FT>& d,
                         const Vector3DT<FT>& p1, const Vector3DT<FT>& p2, const Vector3DT<FT>& p3) {

    // 平面の法線ベクトル
    Vector3DT<FT> normal = (p2 - p1).cross(p3 - p1);
    normal.normalize();
    // 法線との内積
    FT a = (p1 - d).dot(normal);
    if( a > 0 )      { return FrontSide; } // 表
    else if( a < 0 ) { return BackSide; } // 裏
    else               { return OnSurface; } // 平面上
  }


  // xy平面上での直線v1-v2 と点 tの位置関係を符号で返す
  template <typename VtxT1, typename VtxT2>
  inline int SideCheck2D(const VtxT1& v1, const VtxT1& v2, const VtxT2& t) {
   // v1-v2の直線の方程式 ax+by+c = 0;
    VtxT1::value_type a = v2.y - v1.y;
    VtxT1::value_type b = v1.x - v2.x;
    VtxT1::value_type c = -a * v1.x - b * v1.y;
    VtxT1::value_type side = a * t.x + b * t.y + c;
    if (side == static_cast<VtxT1::value_type>(0.0)) return 0;
    return side < 0 ? -1 : 1;
  }


  // 頂点の集合
  // 公差以下のものは丸められる
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

	  // 同一平面上にある点を反時計回りに並び変える
    // 中心座標との内積(なす角)の順でソートする
    // verticesNormal 平面の法線ベクトル
    void aroundSort(const Vector3DT<FloatT>& verticesNormal) {
      // 中心点を求める
      center.clear();
      BOOST_FOREACH(Vector3DT<FloatT>& v, vertexList) {
        center += v;
      }
      center /= static_cast<FloatT>(vertexList.size());
      DEBUGLOG("center=%f,%f,%f\n", center.x, center.y, center.z);

      DEBUGLOG("v0=%f,%f,%f\n", vertexList[0].x, vertexList[0].y, vertexList[0].z);
      Vector3DT<FloatT> basePoint = vertexList[0] - center; // 基準となる最初の点

      // 境界となる平面の法線を求める
      Vector3DT<FloatT> sideNormal = basePoint.cross(verticesNormal);
      basePoint.normalize();


      std::sort(vertexList.begin(), vertexList.end(), Compare(center, basePoint, sideNormal));

      BOOST_FOREACH(Vector3DT<FloatT>& v, vertexList) {
        DEBUGLOG("%f,%f,%f dot=%f\n", v.x, v.y, v.z, basePoint.dot((v - center).normalize()));
      }


    }

    // TRIANGLE_FAN用の頂点リストを生成する
    template <typename Container>
    void makeTriangleFan(Container& container, bool invert = false) {
      // TODO: 頂点リストの最適化を行いたい
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



    // a1,a2を端点とする線分とb1,b2を端点とする線分の交差判定
    template <typename P>
    inline bool IsIntersectedLine2D(const P& a1, const P& a2, const P& b1, const P& b2) {
      return (a2-a1).cross(b1-a1) * (a2-a1).cross(b2-a1) < 1.0e-5f
          && (b2-b1).cross(a1-b1) * (b2-b1).cross(a2-b1) < 1.0e-5f;
    }

    // a1,a2を端点とする線分とrectの四辺との交差判定
    template <typename P, typename R>
    inline bool IsIntersectedLine2D(const P& a1, const P& a2, const R& rect) {
        return IsIntersectedLine2D(a1, a2, P(rect.left, rect.top), P(rect.Right(), rect.top))
            || IsIntersectedLine2D(a1, a2, P(rect.left, rect.Bottom()), P(rect.Right(), rect.Bottom()))
            || IsIntersectedLine2D(a1, a2, P(rect.left, rect.top), P(rect.left, rect.Bottom()))
            || IsIntersectedLine2D(a1, a2, P(rect.Right(), rect.top), P(rect.Right(), rect.Bottom()));
    }

    // a1,a2を端点とする線分とb1,b2を端点とする線分の交点を求める
    template <typename P>
    inline P ClacIntersectPoint2D(const P& a1, const P& a2, const P& b1, const P& b2) {
        P b = b2-b1;
        P::ValueType d1 = abs(b.cross(a1-b1));
        P::ValueType d2 = abs(b.cross(a2-b1));
        P::ValueType t = d1 / (d1 + d2);
        return a1 + ((a2-a1) * t);
    }
    // a1,a2を端点とする線分とrectとの交点を求める
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
