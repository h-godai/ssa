/*
 *  BoostTest.cpp
 *  OpenGLES-test2
 *
 *  Created by IKEDA KOHEI on 11/04/05.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "BoostTest.h"
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>


extern "C"
void boostTest() {

  namespace fs = boost::filesystem;
  
  // ディレクトリ作成
  fs::path dir( "my_dir" );
  fs::create_directory( dir );
  
  // ファイルを開いて書き込みー
  // ディレクトリ名とファイル名の結合は / 演算子で
  fs::ofstream f( dir/"test.txt" );
  f << "Hello!" << std::endl;
  f.close();
}
