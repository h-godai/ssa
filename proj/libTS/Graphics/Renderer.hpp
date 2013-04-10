/* Renderer.h


*/


#ifndef Rendering_H
#define Rendering_H

#include "GLCommon.h"
#include "IRenderer.hpp"
#include "IGraphicsContext.hpp"

namespace ts {


  class Renderer : public IRenderer {
  public:
    Renderer() {}
    virtual ~Renderer() {}

    virtual void initialize(IGraphicsContext::ptr gc) { m_gc = gc; }
    virtual void setGraphicContext(IGraphicsContext::ptr& gc) { m_gc = gc; }


  protected:
    IGraphicsContext::ptr m_gc;
  };
}
#endif
