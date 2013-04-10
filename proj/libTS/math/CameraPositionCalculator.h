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

    // �l���̍��W��ݒ肷��
    void startCalculate(const Vertices4_3Dd& vertex);

    // �J�����ʒu�v�Z
    // startCaculate()���R�[����Ature���Ԃ�܂ŌJ��Ԃ��R�[������
    bool calcCameraPos(const RectS32& viewPort);
	
    // �J�����̈ʒu�v�Z
    bool calcCameraPos(const Vertices4_2D& mapRect, const RectS32& vp);

    // �J�����̐���ʒu�v�Z(�蓮�ݒ�p)
    void calcCamera(float64_t dx, float64_t dy);

    // 2D�摜�̃A�X�y�N�g��𓾂�
    float64_t caleAspect(const Vertices4_2D& mapRect);

    // �J�����v�Z�t�F�[�Y��Ԃ��B0�ŏI���B
    int getCalcPhase() const;

    // �q��ʐ^�̒��S�_�����߂�
    static Vector3Dd getCenterPoint(const Vertices4_3Dd& vertex);

    // �ʐ^�̒��S�_����J�����̐���ʒu�̃x�N�g�������߂�
    Vector3Dd calcCameraVector() const;

  private:
    void operator=(const CameraPositionCalculator&){}

  private:
    CameraPosition& m_camera;      // �J�����̈ʒu
  	float64_t m_imageAspectRatio;
    Vector3Dd m_eye0;             // �v�Z�J�n���̃J�����ʒu
    Vertices4_3Dd m_corners;      // �ʐ^�̒��_���W
    int m_calcPhase;
    Vector3Dd m_ray;
    RectS32 m_viewPort;

  };

} // namespace


#endif
