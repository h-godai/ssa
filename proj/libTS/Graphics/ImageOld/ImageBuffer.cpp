#include "stdafx.h"

#include "ImageBuffer.hpp"


namespace ts {

  ImageBuffer::ptr ImageBuffer::CreateImageBuffer(PixelFormat fmt, uint32_t width, uint32_t height) {
    switch (fmt) {
    case R8G8B8:
      return ImageBuffer::ptr(new ImageBufferRGB(width, height));
    case A8R8G8B8:
      return ImageBuffer::ptr(new ImageBufferARGB(width, height));
    case GRAY8:
      return ImageBuffer::ptr(new ImageBufferG8(width, height));
    default:
      throw std::invalid_argument("ImageBuffer::CreateImageBuffer");
    }
  }

}

