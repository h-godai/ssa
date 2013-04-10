// Test.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <gl/gl.h>

#include "libTS/Graphics/ext/OpenGLRenderer.h"
#include "libTS/Graphics/ext_win32/GraphicsContextWGL.h"
#include "libTS/Graphics/GraphicsObject.hpp"
#include "libTS/Image/ColorCorrect.hpp"

using namespace ts;
using namespace ts::gobj;
using namespace boost;


int _tmain(int argc, _TCHAR* argv[])
{
  gSetDebuglog(TRUE);
  gSetDebuglogStderr(TRUE);

  shared_ptr<IGraphicsContext> wgl(new GraphicsContextWGL);
  shared_ptr<OpenGLRenderer> gl(new OpenGLRenderer(wgl));

  Mesh<ts::Vertex>::ptr mesh1(new Mesh<ts::Vertex>());
  Mesh<ts::VertexWithNormal>::ptr mesh2(new Mesh<ts::VertexWithNormal>());
  Mesh<ts::VertexWithNormalTexCoords>::ptr mesh3(new Mesh<ts::VertexWithNormalTexCoords>());
  shared_ptr<Group> group(new Group());
  group->addChild(mesh1);
  group->addChild(mesh2);
  group->addChild(mesh3);
  group->render(0);
 

  // color correct�̃e�X�g
  image::rgba8_image_t img(512,512);
  ColorCorrect::gamma(img, 1.5);

  vector <image::rgba8_image_t> imagelist;
  // imagelist�ɉ摜��ǂݍ���
  ColorCorrect::BrightnessLeveling(imagelist);
  // image�������o��
  
  printf("Graphics test completed.\n");
  return 0;

}
