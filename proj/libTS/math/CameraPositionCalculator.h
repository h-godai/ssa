// CameraPositionCalculator.hpp
//
//

#ifndef CAMERAPOSITIONCALCULATOR_HPP
#define CAMERAPOSITIONCALCULATOR_HPP

#include "Geometric.hpp"
#include "CameraPosition.h"

namespace ts {

  using ts::Vector3Dd;
  using ts::Matrixd;

  class CameraPositionCalculator {
  public:
    CameraPositionCalculator(CameraPosition& camera, float64_t aspect);

    // 四隅の座標を設定する
    void startCalculate(const Vertices4_3Dd& vertex);

    // カメラ位置計算
    // startCaculate()をコール後、tureが返るまで繰り返しコールする
    bool calcCameraPos(const RectS32& viewPort);
	
    // カメラの位置計算
    bool calcCameraPos(const Vertices4_2D& mapRect, const RectS32& vp);

    // カメラの推定位置計算(手動設定用)
    void calcCamera(float64_t dx, float64_t dy);

    // 2D画像のアスペクト比を得る
    float64_t caleAspect(const Vertices4_2D& mapRect);

    // カメラ計算フェーズを返す。0で終了。
    int getCalcPhase() const;

    // 航空写真の中心点を求める
    static Vector3Dd getCenterPoint(const Vertices4_3Dd& vertex);

    // 写真の中心点からカメラの推定位置のベクトルを求める
    Vector3Dd calcCameraVector() const;

  private:
    void operator=(const CameraPositionCalculator&){}

  private:
    CameraPosition& m_camera;      // カメラの位置
  	float64_t m_imageAspectRatio;
    Vector3Dd m_eye0;             // 計算開始時のカメラ位置
    Vertices4_3Dd m_corners;      // 写真の頂点座標
    int m_calcPhase;
    Vector3Dd m_ray;
    RectS32 m_viewPort;

  };

} // namespace


#endif
