/*
 */

#include <stdio.h>
#include "libTS/appfw/ext_linux/glview.hpp"
#include "libTS/appfw/appfw.hpp"

/* Program entry point */

int main(int argc, char *argv[])
{
  ts::appfw::app_info::ptr info(new ts::appfw::app_info);
  info->app_dir_ = ".";
  info->res_dir_ = ".";
  info->doc_dir_ = ".";

  ts::appfw::glview oglview(argc, argv, info);
  oglview.initializeFrame(ts::math::rect_i(0,0,800,600));


  oglview.run();
  return EXIT_SUCCESS;
}
