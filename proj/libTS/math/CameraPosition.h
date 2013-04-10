

#ifndef CAMERAPOSITION_HPP
#define CAMERAPOSITION_HPP

#include "libTS/Common/Common.h"
#include "libTS/Math/Vector.h"
#include "libTS/Util/bstream.hpp"

namespace ts {

  using ts::Vector3Dd;

  struct CameraPosition {
    Vector3Dd eye;    // ���_
    Vector3Dd target; // �����_
    Vector3Dd top;    // ������̃x�N�g��
    float64_t fovy;     // ����p
    float64_t aspect;   // �c���� (width/height)
    Vector3Dd offset;   // �\���p�̃I�t�Z�b�g
    CameraPosition()
      : eye(0.0,0.0,1230.0)
      , target(0.0,0.0,0.0)
      , top(0.0,0.5,0.5)
      , fovy(90.0)
      , aspect(640.0/480.0)
      , offset(0.0, 0.0, 0.0)
    {
      top.normalize();
    }
    bool operator == (const CameraPosition& c) const {
      return eye == c.eye && target == c.target && top == c.top && fovy == c.fovy && aspect == c.aspect;
    }
    bool operator != (const CameraPosition& c) const { return !(*this == c); }

    // bstream����̓ǂݏo��
    const bstream& deserialize(const bstream& bs) {
      bstream tmp;
      bs >> tmp;
      tmp >> eye;
      tmp >> target;
      tmp >> top;
      tmp >> fovy;
      tmp >> aspect;
      tmp >> offset;
      return bs;
    }
    // bstream�ւ̏�������
    bstream& serialize(bstream& bs) const {
      bstream tmp;
      tmp << eye;
      tmp << target;
      tmp << top;
      tmp << fovy;
      tmp << aspect;
      tmp << offset;
      bs << tmp;
      return bs; 
    }
  };

  // bstream����̓ǂݏo��
  template <> inline    const bstream& operator >> (const bstream& bs, CameraPosition& s) {
    return s.deserialize(bs);
  }
  // bstream�ւ̏�������
  template <> inline    bstream& operator << (bstream& bs, const CameraPosition& s) {
    return s.serialize(bs);
  }
  // bstream�ւ̏�������
  template <> inline    bstream& operator << (bstream& bs, CameraPosition& s) {
    return s.serialize(bs);
  }



}

#endif


