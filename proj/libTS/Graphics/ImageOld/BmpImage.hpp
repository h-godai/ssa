// BmpImege.hpp
//
// Windows BITMAPを扱うクラス



#ifndef TSL_PLATFORMDEPENDENT_BmpIMAGE_HPP
#define TSL_PLATFORMDEPENDENT_BmpIMAGE_HPP

#include <string>
#include "ImageFile.hpp"

namespace ts {
  using std::string;


  class BmpImage : public ImageFile {
  public:
    BmpImage();
    virtual ~BmpImage();

    // virtual method
    virtual ImageFormatType getImageFormat() const { return ImageFile::BMP; }
    // ファイルに書き込む
    virtual bool compress(const string& fileName, const ImageBuffer& input);
    virtual bool deCompress(const string& fileName, ImageBuffer& output);

  };


} // namespace

#endif

