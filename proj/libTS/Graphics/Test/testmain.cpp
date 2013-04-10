#include "stdafx.h"

#include "Graphics/ext/OpenGLRenderer.h"
#if 0
#ifdef WIN32
#include "Graphics/ext_win32/GraphicsContextWGL.h"
#endif
#include "Graphics/GraphicsObject.h"
#endif

using namespace ts;
using namespace ts::gobj;
using namespace boost;

int test_main(int ac, char* av[]) {

#ifdef WIN32
  shared_ptr<IGraphicsContext> wgl(new GraphicsContextWGL);
  shared_ptr<OpenGLRenderer> gl(new OpenGLRenderer(wgl));
#endif
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
