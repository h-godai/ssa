#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "../lib/libpng/png.h"

//#pragma comment(lib,"libpng.lib") 
//#pragma comment(lib,"zlib.lib") 


static void GDR_loadPngTexture_readProc(png_structp png_ptr, 
                                    png_bytep data, 
                                    png_size_t length) {
    //KrcFile* f = (KrcFile*)png_get_io_ptr(png_ptr);
    //f->read(data, length);
    fread(data, length, 1, (FILE *)png_get_io_ptr(png_ptr));
}



  // ファイルからPNGファイルの読み込み
int PNG_deCompress(png_structp png_ptr, png_infop info_ptr, FILE* file, int* w, int* h, int* channel) {
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth, color_type;

	if (setjmp(png_jmpbuf(png_ptr))){
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		return 0;
	}
	png_set_read_fn(png_ptr, file, GDR_loadPngTexture_readProc);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, (int*)(&bit_depth), (int*)(&color_type), NULL, NULL, NULL);

	// パラメータの確認
    if (bit_depth != 8) return 0;
    switch (color_type) {
    case PNG_COLOR_TYPE_GRAY:
      *channel = 1;
      break;
    case PNG_COLOR_TYPE_RGB:
      *channel = 3;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      *channel = 4;
      break;
    default:
      return 0;
    }
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_expand(png_ptr);
		*channel = 4;
	}
    *w = width;
    *h = height;

    return 1;
  }


// 書込みコールバック関数
//static void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
//}

int PNG_compress(FILE* fp, int width, int height, png_byte** row_pointers, int color_type) {
  png_structp png_ptr;
  png_infop info_ptr;
  int bit_depth = 8; // 画像のチャンネル１個分のビット深度(1,2,4,8,16)

  // メモリ確保
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == 0) return 0;

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == 0) {
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    return 0;
  }


  if (setjmp(png_jmpbuf(png_ptr))){
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  // ファイルポインタのセット
  png_init_io(png_ptr, fp);

  // writeコールバックの設定
  //png_set_write_status_fn(png_ptr, write_row_callback);


  // 書込み用関数
  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE, 0, 0);


  // 書き込み
//  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, row_pointers);

  png_write_end(png_ptr, NULL);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  return 1;
}
