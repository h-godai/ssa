

#ifndef CAMERAPOSITION_HPP
#define CAMERAPOSITION_HPP

#include "libTS/Common/Common.h"
#include "libTS/Math/Vector.h"
#include "libTS/Util/bstream.hpp"

namespace ts {

  using ts::Vector3Dd;

  struct CameraPosition {
    Vector3Dd eye;    // 視点
    Vector3Dd target; // 注視点
    Vector3Dd top;    // 上方向のベクトル
    float64_t fovy;     // 視野角
    float64_t aspect;   // 縦横比 (width/height)
    Vector3Dd offset;   // 表示用のオフセット
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

    // bstreamからの読み出し
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
    // bstreamへの書き込み
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

  // bstreamからの読み出し
  template <> inline    const bstream& operator >> (const bstream& bs, CameraPosition& s) {
    return s.deserialize(bs);
  }
  // bstreamへの書き込み
  template <> inline    bstream& operator << (bstream& bs, const CameraPosition& s) {
    return s.serialize(bs);
  }
  // bstreamへの書き込み
  template <> inline    bstream& operator << (bstream& bs, CameraPosition& s) {
    return s.serialize(bs);
  }



}

#endif


