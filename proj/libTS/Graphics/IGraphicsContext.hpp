/* IGraphicsContext.h 

  @author h.godai

	レンダリングする対象のフレームワークを定義します。
	主なフレームワークは以下の様なものがあります


	Windows    console    (自分でWindowを作る)
	Windows    Wnd        (hWnd, CWndなど)
	Windows    fullscreen (全画面)
	Linux/Unix X-Window

*/

#ifndef TS_IGraphicsContext_h
#define TS_IGraphicsContext_h

#include "libTS/Common/Common.h"
#include "libTS/Graphics/GLCommon.h"
#include "libTS/math/rect.hpp"
#ifndef STDAFX_H
#include <set>
#include <boost/shared_ptr.hpp>
#endif

namespace ts {

  // GraphicsContextのインターフェイスクラス
  class IGraphicsContext {
  public:
    typedef boost::shared_ptr<IGraphicsContext> ptr;
    typedef uint32_t ContextId;

    virtual ~IGraphicsContext() {}

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void setViewPort(const ts::math::rect_i&) = 0;
    virtual void setClearColor(const ColorRGBA& col) = 0;
    virtual void deleteTexture(uint32_t) = 0;
    
    virtual ContextId getContextId() const = 0;
  };

  // GraphicsContextの実装ベースクラス
  // IGraphicsContextを継承するクラスは本クラスを継承すること
  class GraphicsContext : public IGraphicsContext {
    typedef std::map<uint32_t, IGraphicsContext*> ContextList;

  protected:
    GraphicsContext()
      : m_contextId(0)
    {
      for (uint32_t id = 0; id < m_contextIdList.size(); ++id) {
        if (m_contextIdList.find(id) == m_contextIdList.end()) {
          m_contextId = id;
          break;
        }
      }
      m_contextIdList[m_contextId] = this;
    }
    virtual ~GraphicsContext() {
      m_contextIdList.erase(m_contextIdList.find(m_contextId));
    }
  public:

    static void DeleteTexture(ContextId id, uint32_t handle) {
      ContextList::iterator found = m_contextIdList.find(id);
      if (found != m_contextIdList.end() && found->second != 0) {
        found->second->deleteTexture(handle);
      }
    }
    virtual ContextId getContextId() const { return m_contextId; }

  protected:
    ContextId m_contextId;

  private:
    static ContextList m_contextIdList;
  };



} // namespace
#endif
