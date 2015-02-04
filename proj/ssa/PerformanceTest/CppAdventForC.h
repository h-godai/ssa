// C++ Advent test -*-tab-width:4-*-

#pragma once
#include <stdint.h>

#define IMAGE_WIDTH 19200
#define IMAGE_HEIGHT 1200

#ifdef __cplusplus
extern "C" {
#endif
void test_bitmap_color_transform_pure_c(unsigned short* buffer);
void test_bitmap_color_transform_macro_c(unsigned short* buffer);
#ifdef __cplusplus
} // extern "C" {

#include <vector>

namespace app {
  //  using std::uint16_t;  

  template <int PixelBits>
  struct pixel_t {};
  template <>
  struct pixel_t<16> {
    uint16_t value_;
    pixel_t() : value_(0) {}
    pixel_t(int r, int g, int b)
        : value_(static_cast<uint16_t>(((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3)))
    {}
    int get_r() const { return (value_ >> 8) & 0xf8; }
    int get_g() const { return (value_ >> 3) & 0xfc; }
    int get_b() const { return (value_ << 3) & 0xff; }
  };
 
  typedef std::vector<pixel_t<16> > frame_buffer_t;
  struct transcolor {
      template <typename Pixel>
      void operator () (Pixel& px) {
          px = Pixel(px.get_g(), px.get_b(), px.get_r());
      }
  };
  void bitmap_color_transform_cpp(frame_buffer_t& buffer);
}

#endif
