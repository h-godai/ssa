
#pragma once

//
// PNG_COLOR_TYPE_PALETTE	  インデックスカラー
// PNG_COLOR_TYPE_RGB		    RGB
// PNG_COLOR_TYPE_RGB_ALPHA	RGBA
//

#include <cstring>
#include <vector>
#include "co_vec2.hpp"

#include <zlib.h>
#if defined (_MSC_VER)
#ifdef _DEBUG
#pragma comment (lib, "zlibMTd.lib")
#else
#pragma comment (lib, "zlibMT.lib")
#endif
#endif

#include <png.h>
#ifndef png_infopp_NULL 
#define png_infopp_NULL NULL
#endif

#if defined (_MSC_VER)
#ifdef _DEBUG
#pragma comment (lib, "libpngMTd.lib")
#else
#pragma comment (lib, "libpngMT.lib")
#endif
#endif

namespace ngs {

const int PNG_COLOR_TYPE_NONE = -1;

struct Palette {
	u_char red, green, blue, alpha;
};

static void readFunc(png_struct *hdl, png_bytep buf, png_size_t size)
{
	u_char **p;

	p = (u_char **)png_get_io_ptr(hdl);
	std::memcpy(buf, *p, size);
	*p += (int)size;
}

class Png {
	Png() {}
public:
	int type;
	ngs::Vec2<int> size;
	u_char *image;																		// イメージ格納ポインタ
	std::vector<Palette> pal;													// パレット格納ポインタ
		
	Png(png_bytep ptr, size_t length) : type(PNG_COLOR_TYPE_NONE), image() {
		if(png_check_sig(ptr, length))
		{
			png_struct *hdl;
			png_info *info = 0;
			hdl = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if(hdl)
			{
				info = png_create_info_struct(hdl);
				if(info)
				{
					png_bytep filepos;
					filepos = ptr;
					png_set_read_fn(hdl, (png_voidp)&filepos, (png_rw_ptr)readFunc);
					png_read_info(hdl, info);

					png_uint_32 width, height;
					int depth;
					png_get_IHDR(hdl, info, &width, &height, &depth, &type, NULL, NULL, NULL);
					size.set(width, height);
						
					if(depth == 16)
					{
						png_set_strip_16(hdl);	// 16bit -> 8bit
					}
					else
					if(depth == 4)
					{
						png_set_packing(hdl);	// 4bit -> 8bit
					}
					else
					if(depth != 8)
					{
						// PRINTF("Format error(%d)\n", depth);
					}

					int trans_ex;
					png_bytep trans;
					png_color_16p trans_16;
					int trans_num;
					trans_ex = png_get_tRNS(hdl, info, &trans, &trans_num, &trans_16);

					png_read_update_info(hdl, info);

					int num_palette = 0;
					png_colorp palette;
					if(png_get_PLTE(hdl, info, &palette, &num_palette))
					{
						// PRINTF("palette: %d\n", num_palette);
						pal.resize(num_palette);
						for(int i = 0; i < num_palette; ++i)
						{
							pal[i].red   = palette[i].red;
							pal[i].green = palette[i].green;
							pal[i].blue  = palette[i].blue;
							if(trans_ex && trans_num)
							{
								pal[i].alpha = *trans;
								++trans;
								--trans_num;
							}
							else
							{
								pal[i].alpha = 255;
							}
						}
					}
						
					size_t w_size = png_get_rowbytes(hdl, info);
					png_byte *img = new png_byte[height * w_size];
					png_bytep *img_array = new png_bytep[height];
					for(png_uint_32 h = 0; h < height; ++h)
					{
						img_array[h] = &img[w_size * h];
					}
					png_read_image(hdl, img_array);
					png_read_end(hdl, info);

					image = new u_char[w_size * height];
					for(png_uint_32 h = 0; h < height; ++h)
					{
						std::memcpy(&image[w_size * h], img_array[h], w_size);
					}
					delete[] img;
					delete[] img_array;
				}
			}
			png_destroy_read_struct(&hdl, &info, NULL);
		}
	}
	~Png() {
		delete[] image;
	}
};

}
