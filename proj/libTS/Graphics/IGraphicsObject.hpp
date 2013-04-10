/* IGraphics.h


  描画オブジェクトのインターフェイス


*/

#ifndef TS_GRAPHICS_IGRAPHICSOBJECT_H
#define TS_GRAPHICS_IGRAPHICSOBJECT_H

#include "libTS/Common/Common.h"
#include "libTS/Util/policy.hpp"

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "libTS/Util/bstream.hpp"

namespace ts {
  class IRenderer;

  // 描画オブジェクトの最小単位
  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class IGraphicsObject : public PointerPolicyT<ts::IGraphicsObject<PointerPolicyT> > {
  public:
    virtual ~IGraphicsObject() {}
    virtual void render(IRenderer* gl) = 0;
    virtual void serialize(bstream& bs) = 0;
    virtual void deserialize(const bstream& bs) = 0;
  };

  typedef IGraphicsObject<>::ptr IGraphicsObject_ptr;


} // namespace
#endif
