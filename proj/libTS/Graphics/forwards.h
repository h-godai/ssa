/* forwaerds.h


  前方宣言用のヘッダ

*/

#ifndef TS_GRAPHICS_FORWARDS_H
#define TS_GRAPHICS_FORWARDS_H

#include <boost/shared_ptr.hpp>
#include "libTS/Util/policy.hpp"

namespace ts {
#ifndef IRenderer_H
  class IRenderer;
#endif
#ifndef TS_IGraphicsContext_h
  class IGraphicsContext;
#endif
#ifndef TS_EXT_OpenGLRenderer_h
  class OpenGLRenderer;
#endif
#ifndef TS_EXTWIN32_GraphicsCOntextWGL_H
  class GraphicsContextWGL;
#endif
#ifndef TS_GRAPHICS_IGRAPHICSOBJECT_H
  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class IGraphicsObject;
#endif
}


#endif
