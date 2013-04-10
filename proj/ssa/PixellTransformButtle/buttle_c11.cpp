#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <chrono>
#include <iostream>
#include <iomanip>
 
using namespace std;
using namespace boost;
 
// BEGIN OF MASTER SOURCE
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
struct transcolor {
  void operator () (uint16_t& val) {
    pixel_t<16>& px = reinterpret_cast<pixel_t<16>&>(val);
    px = pixel_t<16>(px.get_g(), px.get_b(), px.get_r());
 }
};
 
void test_bitmap_color_transform_cpp(vector<uint16_t>& buffer){
 for_each(buffer, transcolor());
}
// END OF MASTER SOURCE
 
