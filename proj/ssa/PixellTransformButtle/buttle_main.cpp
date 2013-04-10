#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <chrono>
#include <iostream>
#include <iomanip>
 
using namespace std;
using namespace boost;

extern "C" void test_bitmap_color_transform_macro_c(uint16_t* buffer, size_t buffersize) ;
extern void test_bitmap_color_transform_cpp(vector<uint16_t>& buffer);

namespace{
  constexpr size_t image_width  = 19200;
  constexpr size_t image_height = 1080;
  constexpr auto num_of_loop = 100ull;
}
 
int main(int ac, char* av[]) {
  

  vector<uint16_t> pixels;
  pixels.resize(image_width * image_height);
  
  using c = chrono::high_resolution_clock;
  c::duration tt1,tt2;
 
  cout << "last:\n";
 
  for(auto n = num_of_loop; n; --n){
    auto t2 = c::now();
    test_bitmap_color_transform_macro_c(&pixels[0], pixels.size());
    tt2 += c::now() - t2;
    auto t1 = c::now();
    test_bitmap_color_transform_cpp(pixels);
    tt1 += c::now() - t1;
    cout
      << "\x1b[12D  "
      << hex
      << setw(8)
      << setfill('0')
      << n
      << flush
      ;
  }
  cout
    << "\x1b[12D  FINISHED"
    << endl;
 
  cout
    << "result c++: "
    << scientific
    << setprecision(3)
    << chrono::duration_cast<chrono::duration<double>>(tt1).count() / num_of_loop
    << endl
    << "result c with macro: "
    << scientific
    << setprecision(3)
    << chrono::duration_cast<chrono::duration<double>>(tt2).count() / num_of_loop
    << endl;
}
