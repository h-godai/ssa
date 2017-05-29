/*
 */

#include <stdio.h>
#include "libTS/appfw/ext_linux/glview.hpp"

/* Program entry point */

int main(int argc, char *argv[])
{
  ts::appfw::glview oglview(argc, argv);

  oglview.initializeFrame(ts::math::rect_i(0,0,800,600));
  oglview.run();
  return EXIT_SUCCESS;
}
