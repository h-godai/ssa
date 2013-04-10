// GroundMap.hpp
//
// 2D地図など
//

#ifndef GROUNDMAP_HPP
#define GROUNDMAP_HPP

#ifndef STDAFX_H
#include <string>
#endif
#include "Common/Common.h"
#include "Graphics/Image/PngImage.hpp"
#include "Graphics/GraphicsObject.h"

namespace app {

  using namespace ts;
  using std::string;

  class GroundMap {
  public:

    GroundMap()
      : m_3Dobj(new gobj::Group)
    {
    }

    void initialize(const string& dataDir) {
      const char* fn = "gsi/tokyo_g8.png";
      ImageBuffer::ptr img(PngImage().deCompress(dataDir + fn));
      if (img.get() == 0) throw std::exception("'gsi/tokyo_g8.png' decompress error");
      const uint32_t texSize = 256;
      const float32_t mpp = 2.5f; // 1ピクセルの大きさ(m)
      uint32_t texXcount = img->getWidth() / 256; // 端数は切り捨て
      uint32_t texYcount = img->getHeight() / 256; // 端数は切り捨て
      for (uint32_t y = 0; y < texYcount; ++y) {
        for (uint32_t x = 0; x < texXcount; ++x) {
          ImageBufferG8::ptr texImg(new ImageBufferG8(texSize, texSize));
          texImg->bitblt(0, 0, img.get(), x * texSize, y * texSize, texSize, texSize);
          gobj::Texture2D::ptr tex(new gobj::Texture2D);
          tex->setImageBuffer(texImg);
          gobj::Rectangle3D::ptr rect(new gobj::Rectangle3D);
          rect->setTexture2D(tex);
          float32_t vx = x * texSize * mpp;
          float32_t vy = -1.0f * y * texSize * mpp;
          float32_t w = texSize * mpp;
          rect->setVertex(0, Vector3D(vx,     vy,     0.0f));
          rect->setVertex(1, Vector3D(vx - w, vy,     0.0f));
          rect->setVertex(2, Vector3D(vx - w, vy - w, 0.0f));
          rect->setVertex(3, Vector3D(vx,     vy - w, 0.0f));
          m_3Dobj->addChild(rect);
          m_rect.push_back(rect);
        }
      }
    }

    gobj::Group::ptr get3DObject() { return m_3Dobj; }
    void setOrigin(const Vector3D& v) {
      Matrix matrix;
      matrix.translate(v);
      m_3Dobj->setTransform(matrix);
    }


  private:
    gobj::Group::ptr m_3Dobj;
    vector<gobj::Rectangle3D::ptr> m_rect;

  };


} // namespace


#endif
