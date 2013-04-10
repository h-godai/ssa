// -*-tab-width:4-*-
// render_control.cpp

#include <stdio.h>
//#include "linux_control.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>

//#include <GL/glext.h>

#include "../drawframe_app.hpp"

namespace {

  const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
  const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

  const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
  const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
  const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat high_shininess[] = { 100.0f };

}

namespace ts {
namespace appfw {
namespace ogl {

  
  render_control_ogl::render_control_ogl()
	: orientation_(unknown_orientation)
  {
	//glGenRenderbuffersOES(1, &renderbuffer_);
	//glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderbuffer_);
  }
  
  
  void render_control_ogl::initialize(uint32_t width, uint32_t height, float scf) {
	
	
	glClearColor(1,1,1,1);
	  //	  glEnable(GL_CULL_FACE);
	//	  glCullFace(GL_BACK);
	
	glDisable(GL_CULL_FACE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	

	glViewport(0, 0, width, height);
	scale_factor_ = scf;
	drawframe_app::app_instance().tap_device_scale_factor_ = scale_factor_;
	drawframe_app::app_instance().gr_.initialize(width, height, scale_factor_);
  }
  
  void render_control_ogl::render(double timestamp) {
	// デバイスの回転をワンテンポ遅らせる
	//do_rotate();
	//glDepthFunc(GL_LESS);
	//glDepthRangef(0.0f, 1.0f);
	//glDepthMask(1);
	drawframe_app::app_instance().draw_frame_hook(timestamp);
  }

  void render_control_ogl::update_animation(double time_stamp) {
	//printf("update_animation %f\n", time_stamp);
  }

  void render_control_ogl::enable_animation(bool enable) {
  }
  void render_control_ogl::on_rotate(device_orientation orientation) {
	TS_DEBUGLOG("on_rotate(%d)\n", orientation);
	if (orientation_ != orientation) {
	  orientation_ = orientation;
	}
  }
  
  void render_control_ogl::do_rotate() {
	switch (orientation_) {
	default:
	case unknown_orientation:
	case orientation_faceup:                 // angle=0
	case orientation_facedown:               // angle=0
	  break;
	case orientation_portrait:               // angle=0
	  drawframe_app::app_instance().on_device_rotate(0.0f);
	  break;
	case orientation_portrait_upside_down:   // angle=180
	  drawframe_app::app_instance().on_device_rotate(180.0f);
	  break;
	case orientation_landscape_left:         // angle=270
	  drawframe_app::app_instance().on_device_rotate(270.0f);
	  break;
	case orientation_landscape_right:        // angle=90
	  drawframe_app::app_instance().on_device_rotate(90.0f);
	  break;
	}
  }
} // ogl
  
namespace gl {
  render_control::ptr render_control_factory::create() {
	return new ts::appfw::ogl::render_control_ogl;
  }
} // gl
}}// ts::appfw

