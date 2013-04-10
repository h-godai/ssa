#include "stdafx.h"

#include "../lib/libpng/png.h"
#include "PngImage.hpp"

#ifdef WIN32
#pragma warning (disable : 4996)
#endif

namespace ts {


  extern "C" int PNG_deCompress(png_structp png_ptr, png_infop info_ptr, FILE* file, int* w, int* h, int* channel);


  // ファイルからPNGファイルの読み込み

  ImageBuffer::ptr PngImage::deCompress(const string& fileName, ImageBuffer::PixelFormat /*fmt*/) {

    int width, height , channel;
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == 0) {
      return ImageBuffer::ptr();
    }
	// ヘッダ読み込み
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return ImageBuffer::ptr();
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return ImageBuffer::ptr();
	}

    if (PNG_deCompress(png_ptr, info_ptr, file, &width, &height, &channel) == 0) {
      // 失敗
	  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      fclose(file);
      return ImageBuffer::ptr();
    }
    ImageBuffer::ptr output;
    switch (channel) {
    case 1:
      output = ImageBuffer::CreateImageBuffer(ImageBuffer::GRAY8, width, height);
      break;
    case 3:
      output = ImageBuffer::CreateImageBuffer(ImageBuffer::R8G8B8, width, height);
      break;
    case 4:
      output = ImageBuffer::CreateImageBuffer(ImageBuffer::A8R8G8B8, width, height);
      break;
    default:
      return ImageBuffer::ptr();
    }

  	unsigned char* pixels = output->getBuffer<unsigned char *>();
	assert(pixels != 0);
    png_bytep* row_pointers = (png_bytep *) malloc(height * sizeof(png_bytep));
	assert(row_pointers != 0);
	for (int row = 0; row < height; row++) {
      row_pointers[row] = &pixels[row * width * channel];
	}
	png_read_image(png_ptr, row_pointers);
	free(row_pointers);

	// クリーンアップ
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose(file);
    
    return output;
  }


  extern "C" int PNG_compress(FILE* fp, int width, int height, png_byte** row_pointers, int color_type);


  // ファイルに書き込む
  bool PngImage::compress(const string& fileName, const ImageBuffer::ptr& input) {

    // ファイル開く
    FILE *fp = fopen(fileName.c_str(), "wb");
    if (!fp) {
        return false;
    }
    int color_type = 0;
    switch (input->getPixelFormat()) {
    case ImageBuffer::A8R8G8B8:   color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
    case ImageBuffer::R8G8B8:     color_type = PNG_COLOR_TYPE_RGB;       break;
    case ImageBuffer::GRAY8:      color_type = PNG_COLOR_TYPE_GRAY;      break;
    default:
      fclose(fp);
      return false;
    }
    png_byte **row_pointers = new png_byte*[input->getHeight()];   // rowアドレスリスト
    for (int row = 0; row < input->getHeight(); ++row) {
      row_pointers[row] = static_cast<png_byte*>(input->getBuffer<uint8_t*>() + (row * input->getPixelBytes() * input->getWidth()));
    }

    int result = PNG_compress(fp, input->getWidth(), input->getHeight(), row_pointers, color_type);
    fclose(fp);
    delete [] row_pointers;
    return result != 0;
  }


}
