#include "libTS/appfw/gl/glfw.hpp"


using namespace ts::appfw;
using namespace ts::appfw::gl;
using namespace ts::math;

int main(int ac, char* av[]) {


  glfw fw;
  //fw.show_image("hoge");
  fw.show_image("hoge", pos2_f(0,0));
  fw.show_image("hoge", "hoge", pos2_f(0,0));

}

