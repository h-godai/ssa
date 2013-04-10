#include "stdafx.h"

#include "TSGraphics/ext/RenderingOpenGL.h"
#include "TSGraphics/ext_win32/GraphicsContextWGL.h"
#include "TSGraphics/GraphicsObject.h"

using namespace ts;
using namespace ts::gobj;
using namespace boost;

int test_main(int ac, char* av[]) {

  shared_ptr<IGraphicsContext> wgl(new GraphicsContextWGL);
  shared_ptr<RenderingOpenGL> gl(new RenderingOpenGL(wgl));

  shared_ptr<Mesh> mesh1(new Mesh());
  shared_ptr<Mesh> mesh2(new Mesh());
  shared_ptr<Mesh> mesh3(new Mesh());
  shared_ptr<Group> group(new Group());
  group->addChild(mesh1);
  group->addChild(mesh2);
  group->addChild(mesh3);
  group->render(0);
  
  return 0;

}
