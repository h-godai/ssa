// TextureAllocator.hpp
//
// 1枚のテクスチャに複数の画像を配置するクラス
//
// @author h.godai

#pragma once

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "Math/Math.hpp"

namespace app {
  using namespace ts;
  using boost::shared_ptr;

  // 複数のテクスチャ画像を配置して一つにまとめるためのクラス
  //
  //  +-----+----+----+  //  +-----+-----+
  //  |  0  |  1 |  4 |  //  |  0  |  1  |   
  //  +-----+----++---+  //  +-----+----++---+
  //  |  2  |  3  |      //  |  2  |  3 |  4 |
  //  +-----+----++      //  +-----+----+----+
  //
  class TextureAllocator {
    struct ImageData {
      MapDirection         mapDir;
      ImageBuffer::ptr image;
      Vector2Di            offset;
      Matrix               matrix;            // 座標を2Dへ変換するマトリクス
      RectF32              rect2D;      // 画像の位置情報

      ImageData()
        : mapDir(MAPDIR_UNKNOWN), offset(0,0)
      {}
      ImageData(MapDirection dir, ImageBuffer::ptr& img, const Matrix& mat, const RectF32& rect )
        : mapDir(dir), image(img), offset(0,0), matrix(mat), rect2D(rect)
      {}
    };

  public:
    TextureAllocator()
      : m_width(0), m_height(0)
    {
    }
    
    void addImage(MapDirection mapDir, ImageBuffer::ptr img, const Matrix& matrix, const RectF32& rect) {
      m_images[mapDir] = ImageData(mapDir, img, matrix, rect);
      calcImageOffset();
    }

    void calcImageOffset() {
      int32_t count = 0;
      int32_t offsetX1 = 0;
      int32_t offsetX2 = 0;
      int32_t offsetY = 0;
      for (map<MapDirection, ImageData>::iterator i = m_images.begin(); i != m_images.end(); ++i) {
        ImageData& data = (*i).second;
        switch (count) {
        case 0: // 最初
          m_width = data.image->getWidth();
          m_height = data.image->getHeight();
          data.offset.x = 0;
          data.offset.y = 0;
          break;
        case 1: // 2個目 ひとつめの右に配置
          data.offset.x = m_width;
          data.offset.y = 0;
          m_width += data.image->getWidth();
          m_height = max(m_height, data.image->getHeight());
          offsetX1 = m_width;
          offsetY = m_height;
          break;
        case 2: // 3個目 一つめの下に配置
          data.offset.x = 0;
          data.offset.y = offsetY;
          offsetX2 = data.image->getWidth();
          m_width = max(m_width, offsetX2);
          m_height += data.image->getHeight();
          break;
        case 3: // 4個目　右下に配置
          data.offset.x = offsetX2;
          data.offset.y = offsetY; 
          offsetX2 += data.image->getWidth();
          m_height = max(m_height, offsetY + data.image->getHeight());
          m_width = max(m_width, offsetX2);
          break;
        case 4: // 5個目
          {
            if (offsetX1 < offsetX2) { // 上に配置
              if (data.image->getHeight() > offsetY) {
                offsetX1 = offsetX2;
              }
              data.offset.x = offsetX1;
              data.offset.y = 0; 
              m_width = max(m_width, offsetX1 + data.image->getWidth());
              m_height = max(m_height, data.image->getHeight());
            }
            else { // 下に配置
              data.offset.x = offsetX2;
              data.offset.y = offsetY; 
              m_width = max(m_width, offsetX2 + data.image->getWidth());
              m_height = max(m_height, offsetY + data.image->getHeight());
            }
          }
          break;
        default:
          break;
        }
        ++count;
      }
    }


    // 頂点情報からテクスチャ座標を設定する
    void setTexCoords(Vector3D* v, TexCoords* tx, uint32_t count) {
      *(tx) = TexCoords(0.0f, 0.0f);
      *(tx+1) = TexCoords(0.0f, 0.0f);
      *(tx+2) = TexCoords(0.0f, 0.0f);
      for (ImageDataList::iterator i = m_images.begin(); i != m_images.end(); ++i) {
        ImageData& data = (*i).second;
        if (!isFrontFace(*v, *(v+1), *(v+2), data.matrix)) { continue; } // 裏面は却下
        RectF32 imgRect(data.offset.x, data.offset.y, data.image->getWidth(), data.image->getHeight());
        RectF32 texOffset(imgRect.left / m_width, imgRect.top / m_height, 
                          imgRect.width / m_width, imgRect.height / m_height);


        TexCoords t1 = getTexCoords(*v,     data.matrix,  data.rect2D, texOffset);
        TexCoords t2 = getTexCoords(*(v+1), data.matrix,  data.rect2D, texOffset);
        TexCoords t3 = getTexCoords(*(v+2), data.matrix,  data.rect2D, texOffset);
        if (calcTextureArea2(t1,t2,t3)*1.2f > calcTextureArea2(*tx, *(tx+1), *(tx+2))) {
          *tx = t1;
          *(tx+1) = t2;
          *(tx+2) = t3;
          if (count > 3) {
            for (uint32_t n = 3; n < count; ++n) {
              *(tx+n) = getTexCoords(*(v+n), data.matrix, data.rect2D, texOffset);
            }
          }
        }
      }
    }

    // 頂点座標からテクスチャ座標を計算する
    TexCoords getTexCoords(const Vector3D& v,
                           const Matrix& mat,
                           const RectF32& rect,
                           const RectF32& texOffset) const {
      TexCoords t;
      Vector3D v2d(mat.mult2d(v));
      t.u = texOffset.left + ((v2d.x - rect.left) / rect.width) * texOffset.width;
      t.v = texOffset.top + (1.0f - ((v2d.y - rect.top) / rect.height)) * texOffset.height;
      return t; 
    }


    // イメージを作成する
    ImageBuffer::ptr makeTextureImage() {
      m_textureImage = ImageBufferARGB::ptr(new ImageBufferARGB(m_width, m_height));
      for (ImageDataList::iterator i = m_images.begin(); i != m_images.end(); ++i) {
        ImageData& data = (*i).second;
        m_textureImage->bitblt(data.offset.x, data.offset.y, data.image.get());
      }
      return m_textureImage;
    }



    // 面の向きを調べる
    bool isFrontFace(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, const Matrix& matrix) {
      Vector2D vec1(matrix.mult2d(v1));
      Vector2D vec2(matrix.mult2d(v2));
      Vector2D vec3(matrix.mult2d(v3));
      return (vec2 - vec1).cross(vec3 - vec1) > 0.0f;
    }
    // テクスチャ面の面積を求める
    float calcTextureArea2(const TexCoords& t1, const TexCoords& t2, const TexCoords& t3) {
      Vector2D v1(t2.u - t1.u, t2.v - t1.v);
      Vector2D v2(t3.u - t2.u, t3.v - t2.v);
      Vector2D v3(t1.u - t3.u, t1.v - t3.v);
      return math::TriangleArea2(v1.length(), v2.length(), v3.length());
    }


    size_t getImageCount() const { return m_images.size(); }
  private:
    typedef map<MapDirection, ImageData> ImageDataList;
    ImageBuffer::ptr m_textureImage;
    map<MapDirection, ImageData> m_images;
    int32_t m_width;
    int32_t m_height;
  };

} // namespace
