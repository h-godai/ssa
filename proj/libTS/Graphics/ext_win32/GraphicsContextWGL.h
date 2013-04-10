/* GraphicsContextWGL.h


*/


#ifndef TS_EXTWIN32_GraphicsCOntextWGL_H
#define TS_EXTWIN32_GraphicsCOntextWGL_H

#include "libTS/Graphics/IGraphicsContext.hpp"
#include "libTS/Graphics/Texture.hpp"

#ifndef TS_PLATFORM_WIN32
#error
#else
#ifndef _AFXDLL
#include <windows.h>
#endif
#endif
#include "GL/gl.h"

namespace ts {

  class GraphicsContextWGL : public GraphicsContext {

  public:
    GraphicsContextWGL()
      : m_handle(0)
      , m_hglrc(0)
      , m_hdc(0)
      , m_clearColor(0,0,0,0)
      , m_lock(0)
    {}
    GraphicsContextWGL(HWND handle)
      : m_handle(handle)
      , m_hglrc(0)
      , m_hdc(0)
      , m_clearColor(0,0,0,0)
      , m_lock(0)
    {}
    virtual ~GraphicsContextWGL() {
      if (m_hglrc != 0) {
        ::wglDeleteContext(m_hglrc);
      }
    }


    virtual void beginFrame() {
      if (m_hdc == 0) return;
      // make it the calling thread's current rendering context
      if (::wglMakeCurrent (m_hdc, m_hglrc) == FALSE) {
        // error
        return;
      }

      
      ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      ::glAlphaFunc(GL_GREATER, 0.1f);
      ::glEnable(GL_ALPHA_TEST);

      // call OpenGL APIs as desired ...
      ::glClearColor(m_clearColor.getRf(), m_clearColor.getGf(), m_clearColor.getBf(), 1.0f);
      ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      ::glEnable(GL_DEPTH_TEST);
      ::glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	  ::glEnable(GL_CULL_FACE);
	  //::glDisable(GL_CULL_FACE);
      ::glLoadIdentity();
    }

    virtual void endFrame() {
      if (m_hdc == 0) return;

      if (++m_lock == 1) {
        for (vector<uint32_t>::iterator i = m_deleteTextures.begin(); i != m_deleteTextures.end(); ++i) {
          GLuint handle = *i;
          ::glDeleteTextures(1, &handle);
        }
        m_deleteTextures.clear();
        --m_lock;
      }

      SwapBuffers(m_hdc);
      // when the rendering context is no longer needed ...  
      // make the rendering context not current 
      wglMakeCurrent (NULL, NULL) ; 
    }
    virtual void setClearColor(const ColorRGBA& col) {
      m_clearColor = col;
    }

    virtual void setViewPort(const ts::RectS32& rect) {
      m_rect = rect;
      ::glViewport(m_rect.left, m_rect.top, m_rect.width, m_rect.height);
    }

    void setHDC(HDC hdc) {
      m_hdc = hdc;
      if (m_hglrc == 0) {
	        initWGL(m_hdc);
      }
    }

    void setContextId(IGraphicsContext::ContextId id) { m_contextId = id; }

    virtual void deleteTexture(uint32_t handle) {
      while (m_lock > 0) Sleep(0);
      ++m_lock;
      m_deleteTextures.push_back(handle);
      --m_lock;
    }


  protected:
    void initWGL(HDC hdc) {

    PIXELFORMATDESCRIPTOR pfd = { 
	sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
	1,                     // version number 
	PFD_DRAW_TO_WINDOW |   // support window 
	PFD_SUPPORT_OPENGL |   // support OpenGL 
	PFD_DOUBLEBUFFER,      // double buffered 
	PFD_TYPE_RGBA,         // RGBA type 
	24,                    // 24-bit color depth 
	0, 0, 0, 0, 0, 0,      // color bits ignored 
	0,                     // no alpha buffer 
	0,                     // shift bit ignored 
	0,                     // no accumulation buffer 
	0, 0, 0, 0,            // accum bits ignored 
	32,                    // 32-bit z-buffer 
	0,                     // no stencil buffer 
	0,                     // no auxiliary buffer 
	PFD_MAIN_PLANE,        // main layer 
	0,                     // reserved 
	0, 0, 0                // layer masks ignored 
    }; 
    int iPixelFormat = ChoosePixelFormat(hdc, &pfd); 
    SetPixelFormat(hdc, iPixelFormat, &pfd); 

	// create a rendering context 
    m_hglrc = wglCreateContext (hdc); 
  }

  private:
    RectS32 m_rect;
    HWND m_handle;
    HGLRC m_hglrc;
    HDC m_hdc;
    ColorRGBA m_clearColor;
    vector<uint32_t> m_deleteTextures;
    volatile int m_lock; // ä»à’mutex

  };

  struct TextureData {
    std::string name_;
    Texture2D::ptr texture_;
    bool isLoaded() const { return (bool)texture_ };
  };
  struct TexturePool {
    std::map<std::string, TextureData> textures_;
    static TexturePool& Instance() {
      static TexturePool self;
      return self;
    }
    Texture2D::ptr load(const std::string& fn) {
      std::map<std::string, TextureData>::iterator found = textures_.find(fn);
      if (found != textures_.end()) {
        return found.second.texture_;
      }

      image::basic_image<image::rgba8_pixel_t> buffer;
      try {
        buffer.loadFromFile(fn);
      }
      catch (const image::exception& ) {
        // ì«Ç›çûÇ›ÉGÉâÅ[
        return Texture2D::ptr();
      }
      Texture2D::ptr texture(new Texture2D(buffer));

      TextureData& td = textures_[fn];
      td.name_ = fn;
      td.texture_ = texture;
      return td.texture_;
    }
  };







} // namespace

#endif
