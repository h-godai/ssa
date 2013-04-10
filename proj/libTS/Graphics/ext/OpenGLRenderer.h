/* RenderingOpenGL.h


*/


#ifndef TS_EXT_RenderingOpenGL_h
#define TS_EXT_RenderingOpenGL_h

#include "libTS/Graphics/Renderer.hpp"
#include "libTS/Graphics/IGraphicsContext.hpp"
#include "libTS/Graphics/GraphicsObject.hpp"
#ifdef TS_PLATFORM_WIN32
#ifndef _AFXDLL
#include <windows.h>
#define TRACE __noop;
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif


namespace ts {

  class OpenGLRenderer : public Renderer {
  public:

    OpenGLRenderer()  {}
    OpenGLRenderer(IGraphicsContext::ptr gc) {
      Renderer::initialize(gc);
    }
    virtual ~OpenGLRenderer() {}

    // IRendererÇÃÉCÉìÉvÉäÉÅÉìÉg
    virtual void initialize(IGraphicsContext::ptr gc) { Renderer::initialize(gc); }
    virtual void finalize() {}
    virtual void onBeginFrame() {}
    virtual void onEndFrame() {}
    virtual bool onBeginScene(IRendererScene* /*scene*/) { return true; }
    virtual void onEndScene() {}
    virtual void onBeginObject(IGraphics*) {}
    virtual void onEndObject(IGraphics*) {}
    virtual void perspective(float64_t fovy, float64_t aspectRatio, float64_t nearPlane, float64_t farPlane) {
      ::glMatrixMode(GL_PROJECTION);
      ::glLoadIdentity();
      ::gluPerspective(fovy, aspectRatio, nearPlane, farPlane);
      ::glMatrixMode(GL_MODELVIEW);

    } 
    virtual void selectProjectionMatrix() {
      ::glMatrixMode(GL_PROJECTION);
    }
    virtual void selectModelViewMatrix() {
      ::glMatrixMode(GL_MODELVIEW);
    }
    virtual void lookAt(const ts::Vector3Dd& eye, const ts::Vector3Dd& center, const ts::Vector3Dd& up) {
      ::glLoadIdentity();
      ::gluLookAt(eye.x,eye.y,eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
    }
    virtual void rotate(float32_t angle, const Vector3D& axis) {
      ::glRotatef(angle, axis.x, axis.y, axis.z);
    }
    virtual void translation(const ts::Vector3D& v) {
      ::glTranslatef(v.x, v.y, v.z);
    }
    virtual void translation(const ts::Vector3Dd& v) {
      ::glTranslated(v.x, v.y, v.z);
    }
    virtual void scale(const ts::Vector3D& scale) {
      ::glScalef(scale.x, scale.y, scale.z);
    }
    virtual void material(const ColorRGB& /*ambientColor*/, const ColorRGBA& /*diffuseColor*/, const ColorRGB& /*emissiveColor*/, const ColorRGB& /*specularColor*/) {
    }
    virtual void shininess(float32_t /*shininess*/) {}
    virtual void color(const ColorRGBA& color) {
      ::glColor4ub(color.r, color.g, color.b, color.a);
    }

    virtual void texture2D(Texture2D& tex, const ColorRGBA& color) {
      if (color.a != 255) {
        ::glEnable(GL_BLEND);
        ::glColor4ub(255, 255, 255, color.a);
      }
      else {
        ::glDisable(GL_BLEND);
        ::glColor4ub(255, 255, 255, 255);
      }

      if (tex.getTextureHandle(m_gc->getContextId()) == Texture2D::NoHandle) {
        GLuint handle = 0;
        ::glEnable(GL_TEXTURE_2D);
        ::glGenTextures(1, &handle);
        GLenum err = glGetError();
	    if (err != GL_NO_ERROR) {
		    ERRORLOG("OpenGL glGenTexturesÇ…é∏îsÇµÇ‹ÇµÇΩÅB%x\n", err);
		    return;
	    }

      tex.setTextureHandle(m_gc->getContextId(), handle);
      glBindTexture(GL_TEXTURE_2D,handle);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      ImageAny& img = tex.getImageBuffer();
      switch (img.pixel_format_type()) {
      case image::rgba8_t:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getBuffer<GLvoid*>());
        break;
      case image::rgb8_t:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), img.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.getBuffer<GLvoid*>());
        break;
      case image::gray8_t:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, img.getWidth(), img.getHeight(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img.getBuffer<GLvoid*>());
        break;
      }
      err = glGetError();
	    if (err != GL_NO_ERROR) {
		    ERRORLOG("OpenGL glGenTexturesÇ…é∏îsÇµÇ‹ÇµÇΩÅB%x\n", err);
		    return;
	    }
        ::glEnable(GL_TEXTURE_2D);
	  }
	  else {
        ::glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex.getTextureHandle(m_gc->getContextId()));
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        GLenum err = glGetError();
	    if (err != GL_NO_ERROR) {
		    ERRORLOG("OpenGL glGenTexturesÇ…é∏îsÇµÇ‹ÇµÇΩÅB%x\n", err);
		    return;
	    }
	  }    
    }
    virtual void drawLine3D(const Vector3D& l1, const Vector3D& l2, const ColorRGBA& color, uint32_t width=1) {
      if (width == 1) {
        ::glDisable(GL_LINE_SMOOTH);
        ::glLineWidth(0.1f);
      }
      else {
        ::glEnable(GL_LINE_SMOOTH);
        ::glLineWidth(static_cast<float32_t>(width)/10.0f);
      }
      ::glDisable(GL_TEXTURE_2D);
      ::glColor4f(color.getRf(), color.getGf(), color.getBf(), color.getAf());
      ::glBegin(GL_LINES);
      ::glVertex3f(l1.x, l1.y, l1.z);
      ::glVertex3f(l2.x, l2.y, l2.z);
      ::glEnd();
    }
    virtual void drawLine2D(const Vector2D& l1, const Vector2D& l2, const ColorRGBA& color, uint32_t width = 1)  {
      ::glDisable(GL_TEXTURE_2D);
      ::glLineWidth(static_cast<float32_t>(width)/10.0f);
      ::glColor4ub(color.r, color.g, color.b, color.a);
      ::glBegin(GL_LINES);
      ::glVertex2f(l1.x, l1.y);
      ::glVertex2f(l2.x, l2.y);
      ::glEnd();
    }

    /// @brief  éläpå`ÇÃïΩñ Çï`âÊÇ∑ÇÈ
    virtual void drawSolidRectangle(const Vertices4_3D& rect, const ColorRGBA& color) {
      ::glDisable(GL_TEXTURE_2D);
      ::glColor4ub(color.r, color.g, color.b, color.a);
      ::glBegin(GL_QUADS);
      ::glVertex3f(rect[0].x, rect[0].y, rect[0].z);
      ::glVertex3f(rect[1].x, rect[1].y, rect[1].z);
      ::glVertex3f(rect[2].x, rect[2].y, rect[2].z);
      ::glVertex3f(rect[3].x, rect[3].y, rect[3].z);
      ::glEnd();
    }



    virtual void pushMatrix() { ::glPushMatrix(); }
    virtual void popMatrix() { ::glPopMatrix(); }
    virtual void transform(const ts::Matrix& matrix) { ::glMultMatrixf(matrix.m); }
    virtual void transform(const ts::Matrixd& matrix) { ::glMultMatrixd(matrix.m); }

    virtual void begin(PrimitiveType primitive) {
      switch(primitive) {
      case LINE:
        ::glBegin(GL_LINES);
        break;
      case LINE_LOOP:
        ::glBegin(GL_LINE_LOOP);
        break;
      case TRIANGLE_STRIP:
        ::glBegin(GL_TRIANGLE_STRIP);
         break;
      case TRIANGLES:
        ::glBegin(GL_TRIANGLES);
        break;
      case QUADS:
        ::glBegin(GL_QUADS);
        break;
      case QUAD_STRIP:
        ::glBegin(GL_QUAD_STRIP);
        break;
      case LINES:
        ::glBegin(GL_LINES);
        break;
      case TRIANGLE_FAN:
        ::glBegin(GL_TRIANGLE_FAN);
        break;
      }
    }
    virtual void end() {
      ::glEnd();
    }

    virtual void vertex(const Vector3D& v) {
      ::glVertex3f(v.x, v.y, v.z);
    }
    virtual void vertex(const Vector2D& v) {
      ::glVertex3f(v.x, v.y, 0.0f);
    }
    virtual void normal(const Vector3D& v) {
      ::glNormal3f(v.x,v.y,v.z);
    }

    virtual void texcoord(const TexCoords& uv) {
      ::glTexCoord2f(uv.u, uv.v);
    }

    virtual void enableTexture2D() {
      ::glEnable(GL_TEXTURE_2D);
    }
    virtual void disableTexture2D() {
      ::glDisable(GL_TEXTURE_2D);
    }
    virtual void lineWidth(float32_t w) {
      ::glLineWidth(w);
    }

    virtual void loadIdentity() {
      ::glLoadIdentity();
    }

    virtual void ortho(float64_t left, float64_t right, float64_t bottom, float64_t top, float64_t zNear, float64_t zFar) {
      ::glOrtho(left, right, bottom, top, zNear, zFar);
    }
    virtual void frustum(float64_t x1, float64_t x2, float64_t y1, float64_t y2, float64_t znear, float64_t zfar) {
      ::glFrustum(x1, x2, y1, y2, znear, zfar);
    }


    virtual void getProjectionMatrix(Matrix& m) {
      ::glGetFloatv(GL_PROJECTION_MATRIX, m.m );
    }
    virtual void getModelViewMatrix(Matrix& m) {
      ::glGetFloatv(GL_MODELVIEW_MATRIX, m.m );
    }

    // DepthBufferÇèâä˙âª
    virtual void clearDepthBuffer() {
      ::glClear(GL_DEPTH_BUFFER_BIT);
    }
    virtual void enablePolygonOffsetFill(bool enable = true) {
      if (enable)
        ::glEnable(GL_POLYGON_OFFSET_FILL);
      else
        ::glDisable(GL_POLYGON_OFFSET_FILL);
    }
    virtual void polygonOffset(float a, float b) {
      ::glPolygonOffset(a, b);
    }
    virtual void enableDepthTest(bool enable = true) {
      if (enable)
        ::glEnable(GL_DEPTH_TEST);
      else
        ::glDisable(GL_DEPTH_TEST);
    }

    virtual void enableAlphaTest(bool enable = true) {
      if (enable) {
        ::glEnable(GL_ALPHA_TEST);
      }
      else {
        ::glDisable(GL_ALPHA_TEST);
      }
    }

    virtual void enableAlphaBlend(bool enable = true) {
      if (enable) {
        ::glEnable(GL_BLEND);
      }
      else {
        ::glDisable(GL_BLEND);
      }
    }
    virtual void enableCullFace(bool enable = true) {
      if (enable) {
        ::glEnable(GL_CULL_FACE);
      }
      else {
        ::glDisable(GL_CULL_FACE);
      }
    }
    virtual void frontCullFace(bool enable = true) {
      ::glCullFace( enable ? GL_FRONT : GL_BACK);
    }


  };


} // namespace
#endif
