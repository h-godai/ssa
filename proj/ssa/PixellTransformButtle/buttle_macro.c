#include <malloc.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
 

#define GET_R(x) ((x >> 8) & 0xf8)
#define GET_G(x) ((x >> 3) & 0xfc)
#define GET_B(x) ((x << 3) & 0xff)
#define TO_RGB(r, g, b) (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3))
 
void test_bitmap_color_transform_macro_c(uint16_t* buffer, size_t buffersize) {
  uint16_t* p = buffer;
  unsigned n;
  for (n = 0; n < buffersize; ++n, ++p) {
      *p = (uint16_t)TO_RGB(GET_G(*p), GET_B(*p), GET_R(*p));
  }
}

 

 
