// C++ Advent test -*-tab-width:4-*-
//#include "stdafx.h"
#ifndef _MSC_VER
#include <unistd.h>
#include <stdint.h>
#else
typedef unsigned short uint16_t;
#endif
#undef new
#undef delete
#undef malloc
#undef free

#include "CppAdventForC.h"


// RGB 5:6:5 ‚Ì16bit bitmap  rrrr rggg gggb bbbb
int get_r(uint16_t x) { return ((x >> 8) & 0xf8); }
int get_g(uint16_t x) { return ((x >> 3) & 0xfc); }
int get_b(uint16_t x) { return ((x << 3) & 0xff); }
uint16_t to_rgb(int r, int g, int b) {
  return (uint16_t)(((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3));
}

void test_bitmap_color_transform_pure_c(uint16_t* buffer) {
  uint16_t* p = buffer;
  int n;
  for (n = 0; n < IMAGE_HEIGHT * IMAGE_WIDTH; ++n, ++p) {
    int r = get_r(*p);
    int g = get_g(*p);
    int b = get_b(*p);
    *p = to_rgb(g, b, r);
  }
}

// RGB 5:6:5 ‚Ì16bit bitmap  rrrr rggg gggb bbbb
#define GET_R(x) ((x >> 8) & 0xf8)
#define GET_G(x) ((x >> 3) & 0xfc)
#define GET_B(x) ((x << 3) & 0xff)
#define TO_RGB(r, g, b) (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3))

void test_bitmap_color_transform_macro_c(uint16_t* buffer) {
  uint16_t* p = buffer;
  int n;
  for (n = 0; n < IMAGE_HEIGHT * IMAGE_WIDTH; ++n, ++p) {
      int r = GET_R(*p);
      int g = GET_G(*p);
      int b = GET_B(*p);
      *p = (uint16_t)TO_RGB(g, b, r);
  }
}
