// ImageFIle.hpp
//
// jpegやpng等の画像ファイルを扱うためのスーパークラス




#ifndef TSL_PLATFORMDEPENDENT_IMAGEFILE_HPP
#define TSL_PLATFORMDEPENDENT_IMAGEFILE_HPP

#include <string>
#include "Util/bstream.hpp"
#include "ImageBuffer.hpp"

namespace ts {
  using std::string;

  class ImageFile {

  public:
    enum ImageFormatType {
      RAW,
      JPEG,
      BMP,
      PNG,
      GIF
    };
    virtual ~ImageFile() {}

    static ImageBuffer::ptr LoadFromFile(const string& fname, ImageBuffer::PixelFormat fmt = ImageBuffer::A8R8G8B8);
    static bool SaveToFile(const string& fname, const ImageBuffer::ptr& input);

    // virtual method
    virtual ImageFormatType getImageFormat() const = 0;
    // ファイルに書き込む
    virtual bool compress(const string& fileName, const ImageBuffer::ptr& input) = 0;
    // 画像イメージを指定したフォーマットに読み込む
    // 失敗した場合は null ptrが返る
    virtual ImageBuffer::ptr deCompress(const string& fileName, ImageBuffer::PixelFormat fmt = ImageBuffer::A8R8G8B8) = 0;


  };

 



}

#endif











