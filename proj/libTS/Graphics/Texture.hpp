#ifndef TS_GRAPHICS_TEXTURE_HPP
#define TS_GRAPHICS_TEXTURE_HPP

#ifndef STDAFX_H
#include <vector>
#endif

#include "libTS/Common/Common.h"
#include "libTS/Util/policy.hpp"
#include "libTS/Image/image.hpp"
#include "libTS/Image/Color.hpp"
#include "IGraphicsContext.hpp"

namespace ts {

#if 0
  class Image2D   {
  public:
    Image2D() {}
    Image2D(ImageAny& image)
      : m_imageBuffer(image) {
    }
    ImageAny& getImage() { return m_imageBuffer; }
    void setImage(ImageAny& image) { m_imageBuffer = image; }
    int getWidth() const { return m_imageBuffer.getWidth(); }
    int getHeight() const { return m_imageBuffer.getHeight(); }
    image::pixel_format_type_t getPixelFormat() { return m_imageBuffer.pixel_format_type(); }
    template <typename BufT>
    BufT getBuffer() { return m_imageBuffer.getBuffer<BufT>(); }
  private:
    ImageAny m_imageBuffer;
  };
#endif


  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class Texture2DT : public PointerPolicyT<Texture2DT<PointerPolicyT> > {
  public:
    static const uint32_t NoHandle = 0xffffffff;

  private:
    struct TextureHandle {
      uint32_t handle;
      TextureHandle(uint32_t h = NoHandle) { handle = h; }
    };

  public:
    Texture2DT()
      : m_modified(false)
    {}
    Texture2DT(ImageAny image) 
      : m_imageBuffer(image)
      , m_modified(true)
    {
    }
    virtual ~Texture2DT() {
      deleteTextures();
    }
    void deleteTextures() {
      for (uint32_t n = 0; n < m_textureHandle.size(); ++n) {
        if (m_textureHandle[n].handle != NoHandle) {
          GraphicsContext::DeleteTexture(static_cast<IGraphicsContext::ContextId>(n), m_textureHandle[n].handle);
        }
      }
      m_textureHandle.clear();
    }


    ImageAny& getImageBuffer() { return m_imageBuffer; }
    void setImageBuffer(ImageAny image) { 
      deleteTextures();
      m_imageBuffer = image; 
      m_modified = true;
    }
    image::pixel_format_type_t getPixelFormat() { return m_imageBuffer.pixel_format_type(); }
    void setColor(const ts::ColorRGBA& color) { m_blendColor = color; }


    void setTextureHandle(IGraphicsContext::ContextId gcId, uint32_t handle) {
      if (gcId >= m_textureHandle.size()) { m_textureHandle.resize(gcId+1); }
      m_textureHandle[gcId] = handle; 
    }
    uint32_t getTextureHandle(IGraphicsContext::ContextId gcId) const {
      if (gcId >= m_textureHandle.size()) { return NoHandle; }
      return m_textureHandle[gcId].handle; 
    }

    const ColorRGBA& blendColor() const { return m_blendColor; }

    //virtual void doRendering(IRenderer* gl) {
    //  gl->texture2D(*this, m_blendColor);
    //}

    bool isModified() const { return m_modified; }
    void setModified(bool modified = true) { m_modified = modified; }

  private:
    ImageAny m_imageBuffer;


    ColorRGBA m_blendColor;
    bool m_blending;
    bool m_wrappingS;
    bool m_wrappingT;
    bool m_levelFilter;
    bool m_imageFilter;
    uint32_t m_uniqID;
    std::vector <TextureHandle> m_textureHandle;
    bool m_modified;
  };



  typedef Texture2DT<> Texture2D;









} // namespace
#endif
