// CameraPositionCalculator.cpp
//
//

#include <math.h>
#include "libTS/Common/Common.h"
#include "CameraPositionCalculator.h"
#include "Matrix.h"

#pragma warning (disable : 4996)

using namespace ts;
namespace {
	// local instance
    float64_t m_val64;
    float64_t m_nice;
    float64_t m_niceRange;
    float64_t m_aspect;
    float64_t m_dir;
    float64_t m_dir2;
    float64_t m_dz;
    Vector3Dd m_p1, m_p2;
    float64_t m_role;
}

namespace ts {

  using ts::Vector3Dd;
  using ts::Matrixd;

  CameraPositionCalculator::CameraPositionCalculator(CameraPosition& camera, float64_t aspect)
    : m_camera(camera)
    , m_imageAspectRatio(aspect)
    , m_calcPhase(0)
	{}

  // 航空写真の四隅の座標を設定する
  void CameraPositionCalculator::startCalculate(const Vertices4_3Dd& vertex) {
    copy(m_corners, vertex);
    m_calcPhase = 7;//1;
    m_val64 = 0.0;
    m_dir2 = 0.001f;
    m_role = 999.0;
 	  m_ray = m_camera.eye - m_camera.target;

  }


  // カメラ位置計算
  // startCaculate()をコール後、tureが返るまで繰り返しコールする
  bool CameraPositionCalculator::calcCameraPos(const RectS32& viewPort) {
    Matrixd matrix;
    matrix.identity();
    matrix.perspective(m_camera.fovy, static_cast<float64_t>(viewPort.width)/viewPort.height, 5.0, 5000.0);
    matrix.lookAt(m_camera.eye, m_camera.target, m_camera.top);

    Vertices4_2D mapRect;
    for (int n = 0; n < 4; ++n) {
      Vector3D r;
      r = m_corners[n];
      r = matrix.mult2d(r);
      r.z = 990.0f;
      mapRect[n].x = r.x;
      mapRect[n].y = r.y;
    }
    return calcCameraPos(mapRect, viewPort);
  }

  // カメラの位置計算
  bool CameraPositionCalculator::calcCameraPos(const Vertices4_2D& mapRect, const RectS32& vp) {
    m_viewPort = vp;
    Vector2Dd l1 = mapRect[1] - mapRect[0];
    Vector2Dd l2 = mapRect[2] - mapRect[1];
    Vector2Dd l3 = mapRect[3] - mapRect[2];
    Vector2Dd l4 = mapRect[0] - mapRect[3];
    switch (m_calcPhase) {
    case 1: // 2D画像を平行四辺形にする
      {
        m_camera.target = getCenterPoint(m_corners);
        m_camera.eye = calcCameraVector();
        m_eye0 = m_camera.eye;

        // 視線のベクトル
  	  m_ray = m_eye0 - m_camera.target;
      m_camera.top = Vector3Dd(0.0, 0.0, 1.0);

        // 視点１
        m_camera.eye = m_camera.target + (m_ray * 0.2);
        m_camera.eye.z = 300.0f; // 300mから徐々に上にもっていく
        m_calcPhase = 2;
        m_nice = 999.9f;
        m_dir = 0.2;
        m_dz = 20.0;
        m_aspect = 10.0;
        m_val64 = 0.1;
        m_dir2 = 10.0;
        m_niceRange = 0.0001;
        m_p1.clear();
        m_p2.clear();
      }
    break;
    case 2: // 高さを変化させながら最適な位置を見つける
    case 4:
      {
        float64_t newNice = fabs(l1.length() - l3.length());
        if (newNice < m_niceRange || fabs(m_dz) < m_niceRange/10.0) {
          // OK!
          if (m_calcPhase == 4) {
            // 2回目だったらここで終了
            ++m_calcPhase;
            break;
          }
          // aspectが2/3を境とする2つのポイントを取得する
          if (caleAspect(mapRect) < m_imageAspectRatio) {
            m_p1 = m_camera.eye;
          }
          else {
            m_p2 = m_camera.eye;
          }
          // ポイントを2つ取得したら終了
          if (!m_p1.isZero() && !m_p2.isZero()) {
            m_calcPhase = 3;
            break;
          }
          m_dir += 0.05;
          float64_t z = m_camera.eye.z;
          m_camera.eye = m_camera.target + (m_ray * m_dir);
          m_camera.eye.z = z;
          m_nice = 999.9f;
          m_dz = 30.0;
          break;
        }
        if (newNice > m_nice) {
          // 悪い傾向
          m_dz *= -0.8777f;
        }
        m_nice = newNice;
        m_camera.eye.z -= m_dz;
        if (m_camera.eye.z < 1.0f) m_camera.eye.z = 1.0f;
      }
      break;
    case 3: // 四角形のアスペクト比を2/3に近づける
    case 5:
      {
        float64_t newAspect = fabs(m_imageAspectRatio - caleAspect(mapRect));
        if (newAspect < m_niceRange || fabs(m_dir2) < m_niceRange) {
          ++m_calcPhase;
          if (m_calcPhase == 6) {
            m_dir = 1.0;
            m_val64 = 0.0;
            m_nice = 999.0;
            m_eye0 = m_camera.eye;
          }
          break;
        }
        if (newAspect > m_aspect) {
          // 悪い傾向
          m_dir2 *= -0.670f;
        }
        m_aspect = newAspect;          
        m_val64 += m_dir2;
        // 直線の方程式
        // (x-p1.x)/(p2.x-p1.x)=(y-p1.y)/(p2.y-p1.y)=(z-p1.z)/(p2.z-p1.z)
        Vector3Dd v(m_p2 - m_p1);
        v.normalize();
        m_camera.eye = m_p1 + (v * m_val64);
      }
      break;

    case 6: // 角度を直角にする
      {
        Vector2Dd x1 = mapRect[2] - mapRect[0];
        Vector2Dd x2 = mapRect[3] - mapRect[1];
        x1.x *= vp.width / vp.height;
        x2.x *= vp.width / vp.height;
        float64_t crossRatio = //fabs(l1.length() - l3.length()) 
                           //+ fabs(l2.length() - l4.length()) 
                            fabs(x1.length() - x2.length());
//        TRACE("crossRatio:%.3f, %.3f\n", crossRatio, m_dir);
        if (crossRatio < m_niceRange*10.0 || m_dir > 720.0) {
          if (m_dir > 720.0) {
            // 仕方ないのでベストを採用
            Vector2Dd r(m_val64/3.0,0.0);
            r.rotate(m_val64);
            m_camera.eye.x = m_eye0.x + r.x;
            m_camera.eye.y = m_eye0.y + r.y;
          }
          ++m_calcPhase;
          m_val64 = 0.0;
          m_dir2 = 0.001f;
          m_role = 999.0;
          break;
        }
        if (crossRatio < m_nice) {
          // ベスト
          m_val64 = m_dir;
          m_nice = crossRatio;
        }
        // 視点を注視点を軸に左右に振る
        Vector2Dd r(m_dir/3.0 ,0.0);
        r.rotate(m_dir);
        m_camera.eye.x = m_eye0.x + r.x;
        m_camera.eye.y = m_eye0.y + r.y;
        m_dir += 1.0;
#if 0
        m_ray = m_camera.eye - m_camera.target;
        Vector2Dd r(m_ray);
        m_val64 += m_dir;
        r.rotate(m_val64);
        m_ray.x = r.x;
        m_ray.y = r.y;
        m_camera.eye.x = m_camera.target.x + r.x;
        m_camera.eye.y = m_camera.target.y + r.y;
#endif
      }
      break;
    case 7: // カメラの傾きを合わせる
  	  {
        float64_t th = atan2(mapRect[0].y - mapRect[1].y, mapRect[0].x - mapRect[1].x) / 2.0;
        Matrixd mat;
        mat.identity();
        mat.rotate(th * Rad2Pai, m_ray);
        m_camera.top = mat.mult(m_camera.top);
        if (fabs(th) < 0.001) {
          m_calcPhase = 8;
        }
        break;
      }

    case 8:
      // おしまい
      m_calcPhase = 0;
    default:
      return true;
    }
    return false;
  }

  // カメラの推定位置計算(手動設定用)
  void CameraPositionCalculator::calcCamera(float64_t dx, float64_t dy) {
    m_camera.eye.z -= dy;
    if (m_camera.eye.z < 1.0f) m_camera.eye.z = 1.0f;
    // v1-v2の直線の方程式 ax+by+c = 0;
    float64_t a = m_eye0.y - m_camera.target.y;
    float64_t b = m_camera.target.x - m_eye0.x;
    float64_t c = -a * m_camera.target.x - b * m_camera.target.y;
    m_camera.eye.y += dx;
    m_camera.eye.x = (-b * m_camera.eye.y - c)/a;
  }

  // 2D画像のアスペクト比を得る
  float64_t CameraPositionCalculator::caleAspect(const Vertices4_2D& mapRect) {
    Vector2Dd l1(mapRect[1] - mapRect[0]);
    Vector2Dd l2(mapRect[2] - mapRect[1]);
    return (l2.length() * m_viewPort.height) / (l1.length() * m_viewPort.width);
  }

  // カメラ計算フェーズを返す。0で終了。
  int CameraPositionCalculator::getCalcPhase() const {
    return m_calcPhase;
  }

  // 航空写真の中心点を求める
  Vector3Dd CameraPositionCalculator::getCenterPoint(const Vertices4_3Dd& vertex) {
    return Vector3Dd::getCrossPoint2D(vertex[0], vertex[2], vertex[1], vertex[3]);
  }

  // 写真の中心点からカメラの推定位置のベクトルを求める
  Vector3Dd CameraPositionCalculator::calcCameraVector() const {
    Vector3Dd center(getCenterPoint(m_corners));
    Vector3Dd camxy(Vector3Dd::getCrossPoint2D(m_corners[0], m_corners[3], m_corners[1], m_corners[2]));
    double fl = m_corners[0].distance(m_corners[1]);
    double nl = m_corners[3].distance(m_corners[2]);
    double a = (fl*1.41421356)/nl;
    double th = atan(a);
    double h = th * camxy.distance(center);
    camxy.z = h;
    return camxy;
  }


} // namespace

