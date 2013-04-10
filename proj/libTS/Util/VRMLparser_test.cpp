// VRMLparser_test.cpp
//
// VRMLparsermの単体テストプログラム
#ifndef STDAFX_H
#include <boost/foreach.hpp>
#include <string>
#include <sstream>
#endif
#include "util.hpp"
#include "VRMLparser.hpp"

#ifdef WIN32
#pragma warning (disable : 4996)
#endif

using namespace std;
using namespace boost;
using namespace ts;
using namespace ts::vrml;

namespace {
  const char vrlmSampleText[] = 
"#VRML V2.0 utf8\n"
"DEF V2Grp01 Transform {\n"
"  translation 0 0 0\n"
"  rotation -1 0 0 -1.571\n"
"  children [\n"
"    DEF V2Grp02 Transform {\n"
"      translation 0 0 0\n"
"      rotation -1 0 0 -1.571\n"
"      children [\n"
"        DEF V2Xfm01 Transform {\n"
"          translation 0 0 0\n"
"          children [\n"
"            DEF VIFS01 Transform {\n"
"              translation 0 0 0\n"
"              children [\n"
"                DEF buiildE Transform {\n"
"                  translation 0 0 0\n"
"                  children [\n"
"                    Shape {\n"
"                      appearance Appearance {\n"
"                        material Material {\n"
"                          diffuseColor 0.8 0.8 0.8\n"
"                          ambientIntensity 0.2\n"
"                          specularColor 0 0 0\n"
"                          shininess 0.05693\n"
"                          transparency 0\n"
"                        }\n"
"                        texture ImageTexture {\n"
"                          url \"buildE.wrl_0.png\"\n"
"                        }\n"
"                      }\n"
"                      geometry DEF buiildE-FACES IndexedFaceSet {\n"
"                        ccw TRUE\n"
"                        solid TRUE\n"
"                        coord DEF buiildE-COORD Coordinate { point [\n"
"                          4.791 -27.881 11.92, 11.68 -27.882 -6.339, \n"
"                          11.68 0 -6.339, 11.68 0 -6.339, 4.792 0 11.92, \n"
"                          4.793 -27.883 11.92, 11.68 -27.884 -6.339, \n"
"                          -4.84 -27.885 -11.92]\n"
"                        }\n"
"                        texCoord DEF buiildE-TEXCOORD TextureCoordinate { point [\n"
"                          0.3241 0.5, -0.002019 0.5, -0.002019 0, \n"
"                          -0.002019 0, 0.3241 0, 0.3241 0.5, \n"
"                          0.6632 0.5]\n"
"                        }\n"
"                        coordIndex [0, 1, 2, -1, 3, 4, 5, -1]\n"
"                        texCoordIndex [  0, 1, 2, -1, 3, 4, 5, -1]n"
"                        }\n"
"                    }\n"
"                  ]\n"
"                }\n"
"              ]\n"
"            }\n"
"          ]\n"
"        }\n"
"      ]\n"
"    }\n"
"  ]\n"
"}\n"
;


}


namespace ts  {
  using namespace vrml;

  bool VRMLparser:: unittest(Unittest* test) {
    test->message("*** start VRMLparser unittest ***\n");
    stringstream ss(vrlmSampleText);
    Parse(ss, *test);
    return true;
  }


//  POINTCUT_UNITTESET_REGIST(VRMLparser);


}


