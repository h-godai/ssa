// ImageFile.cpp

#include "stdafx.h"
#include "Util/ustring.hpp"
#include "ImageFile.hpp"
#include "JpegImage.hpp"
#include "PngImage.hpp"
#include "BmpImage.hpp"

namespace ts {

  ImageBuffer::ptr ImageFile::LoadFromFile(const string& fname, ImageBuffer::PixelFormat fmt) {
    mstring fn(fname);
    fn.tolower();
    if (fn.rightMatch(".jpg") || fn.rightMatch(".jpeg")) {
       return JpegImage().deCompress(fname.c_str(), fmt);
    }
    if (fn.rightMatch(".png")) {
       return PngImage().deCompress(fname.c_str(), fmt);
    }
    // �g���q�s���̂��̂�jpeg���g���C
    return JpegImage().deCompress(fname.c_str(), fmt);
  }
  
  
  bool ImageFile::SaveToFile(const string& fname, const ImageBuffer::ptr& input) {
    mstring fn(fname);
    fn.tolower();
    if (fn.rightMatch(".jpg") || fn.rightMatch(".jpeg")) {
       return JpegImage().compress(fname, input);
    }
    // �ۑ��͂��ׂ�png
    return PngImage().compress(fname, input);
  }

}