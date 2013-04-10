#include <malloc.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
 
#define IMAGE_HEIGHT      1920
#define IMAGE_WIDTH       1080
 
// NOTE: THERE ARE BAD DEFINE USAGE.
#define short uint16_t
#define int   unsigned
 
// BEGIN OF MASTER SOURCE
#define GET_R(x) ((x >> 8) & 0xf8)
#define GET_G(x) ((x >> 3) & 0xfc)
#define GET_B(x) ((x << 3) & 0xff)
#define TO_RGB(r, g, b) (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3))
 
void test_bitmap_color_transform_macro_c(short* buffer) {
  short* p = buffer;
  int n;
  for (n = 0; n < IMAGE_HEIGHT * IMAGE_WIDTH; ++n, ++p) {
      int r = GET_R(*p);
      int g = GET_G(*p);
      int b = GET_B(*p);
      *p = (short)TO_RGB(g, b, r);
  }
}
// END OF MASTER SOURCE
 
// NOTE: UNDEFINE BAD DEFINES.
#undef int
#undef short
 
#define NUM_OF_LOOP 1000
 
int main(){
 
  uint16_t* pixels = calloc(
    IMAGE_WIDTH * IMAGE_HEIGHT,
    sizeof( uint16_t )
  );
 
  unsigned n;
  clock_t t, tt;
  
  setvbuf(stdout, NULL, _IONBF, 0);
  puts("last:");
 
  for(n = NUM_OF_LOOP, tt = 0; n; --n){
    t = clock();
    test_bitmap_color_transform_macro_c( pixels );
    tt += clock() - t;
    printf("\x1b[12D  %08X", n);
  }
  puts("\x1b[12D  FINISHED\n");
 
  free(pixels);
 
  printf(
    "result: %.3e [sec/function]\n",
    (double)tt / CLOCKS_PER_SEC / NUM_OF_LOOP
  );
 
  return 0;
}