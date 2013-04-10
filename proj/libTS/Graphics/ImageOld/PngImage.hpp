// PngImage.hpp
//
// PNG画像ファイルを扱うクラス



#ifndef TSL_PLATFORMDEPENDENT_PNGIMAGE_HPP
#define TSL_PLATFORMDEPENDENT_PNGIMAGE_HPP

#ifndef STDAFX_H
#include <string>
#endif
#include "ImageFile.hpp"

namespace ts {
  using std::string;


  class PngImage : public ImageFile {
  public:
    PngImage() {};
    virtual ~PngImage() {};

    // virtual method
    virtual ImageFormatType getImageFormat() const { return ImageFile::PNG; }

    // ファイルに書き込む
    virtual bool compress(const string& fileName, const ImageBuffer::ptr& input);
 
    // 与えられたバッファのフォーマットに読み込む

    // 画像イメージのフォーマットのまま読み込む
    virtual ImageBuffer::ptr deCompress(const string& fileName, ImageBuffer::PixelFormat fmt = ImageBuffer::A8R8G8B8);
  };


} // namespace

#endif

