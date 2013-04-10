/* layer.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
   layer class
 */

#ifndef TS_APPFW_GL_LAYER_HPP
#define TS_APPFW_GL_LAYER_HPP

#ifdef GLUT
#define glOrthof glOrtho
#endif

#include "context.hpp"

namespace ts {
namespace appfw {
namespace gl {

  struct layer_base {
	typedef shared_ptr<layer_base> ptr;
	uint32_t number_; // レイヤー番号
	layer_base(uint32_t n) : number_(n) {}
	virtual ~layer_base() throw() {}
	virtual void begin_frame(context& gc) = 0;
	virtual void end_frame(context& gc) = 0;
  };

  template <typename ProjectionPolicy>
  struct layer : layer_base {
	typedef shared_ptr<layer<ProjectionPolicy> > ptr;
	ProjectionPolicy proj_;
	bool enableDepthTest_;
	layer(uint32_t num) : layer_base(num), enableDepthTest_(false) {}
	virtual ~layer() throw() {}
	void begin_frame(context& gc) {
	  proj_.begin_frame(gc);
	  if (enableDepthTest_) {
		glEnable(GL_DEPTH_TEST);
	  }
	  else {
		glDisable(GL_DEPTH_TEST);
	  }
	}
	void end_frame(context& gc) {
	  proj_.end_frame(gc);
	}
  };

  struct projection_ortho {
	float rotate_;
	float ofsx_;
	float ofsy_;;
	projection_ortho() : rotate_(-999.0f), ofsx_(0.0f), ofsy_(0.0f) {}

	void begin_frame(context& gc) {
	  //glEnable(GL_CULL_FACE);
	  //glCullFace(GL_FRONT);
	  glDisable(GL_CULL_FACE);


	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();

	  float ofsx = -gc.screen_size().width_/2.0f;
	  float ofsy = -gc.screen_size().height_/2.0f;
	  if (rotate_ == -999.0f) {
		// first time
		rotate_ = gc.rotate();
		ofsx_ = ofsx;
		ofsy_ = ofsy;
	  }

	  if (gc.rotate()- rotate_ > 180.0f) rotate_ += 360.0f;
	  if (gc.rotate() - rotate_ < -180.0f) rotate_ -= 360.0f;

	  rotate_ = rotate_  * 0.9f +  gc.rotate() * 0.1f;
	  ofsx_ = ofsx_ * 0.95f + ofsx * 0.05f;
	  ofsy_ = ofsy_ * 0.95f + ofsy * 0.05f;

	  //glOrthof(0.0f, (float)gc.screen_size_.width_, (float)gc.screen_size_.height_, 0.0f, -1.0f, 1.0f);
	  float w = (float)gc.device_screen_size().width_;
	  float h = (float)gc.device_screen_size().height_;
	  glOrthof(-1.0f, 1.0f, 1.0f, -1.0f,  -1.0f, 1000.0f);
	  glScalef(2.0f/w,  2.0f/h, 1.0f);

	  glRotatef(-rotate_, 0.0f,0.0f,1.0f);
	  glTranslatef(ofsx_, ofsy_, 0.0f);




	  glMatrixMode(GL_MODELVIEW);
	  glLoadIdentity();

#if 0
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
    glDisable(GL_DEPTH_TEST);
    glColor4d(1,1,1,1);
    glPushMatrix();
	glTranslated(400.0,300.0,0.0);
	glRotated(60,1,0,0);
	glRotated(a,0,0,1);
	glutSolidSphere(100,16,16);
    glPopMatrix();
}
#endif
	}
	void end_frame(context& gc) {
	}
  };
  struct projection_parspective {
	void begin_frame(context& gc) {
	}
	void end_frame(context& gc) {
	}
  };

  typedef layer<projection_ortho> ortho_layer;






}}} // ts::appfw::gl
#endif
