// -*-tab-width:4-*-
// render_control.cpp

#include <stdio.h>
#include "ios_control.hpp"
#include "render_control.hpp"
#include "../drawframe_app.hpp"

namespace ts {
namespace appfw {
  namespace mac {
	
	render_control_ios::render_control_ios() 
	  : using_depth_buffer_(false)
	  , orientation_(unknown_orientation)
	  , orientation_countdown_(0)
		
	{
	  
	  
	  glGenRenderbuffersOES(1, &renderbuffer_);
	  glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderbuffer_);
	  
	  
	}
    
  
	void render_control_ios::initialize(uint32_t width, uint32_t height, float scf) {


      // initialize for OpenGL
	  width *= scf;
	  height *= scf;

	  if (using_depth_buffer_) {
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		GLint backingWidth, backingHeight;
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
	  }

      glGenFramebuffersOES(1, &framebuffer_);
      glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer_);

      glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES
                                   , GL_COLOR_ATTACHMENT0_OES
                                   , GL_RENDERBUFFER_OES
                                   , renderbuffer_);

	  if (using_depth_buffer_) {
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	  }

      glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderbuffer_);

	  //glEnable(GL_DEPTH_TEST);
	  //glDepthFunc(GL_LESS);
	  //glDepthRangef(0.0f, 1.0f);
	  //glDepthMask(1);
	  //glBindRenderbufferOES


      glViewport(0, 0, width, height);
	  scale_factor_ = scf;
	  // iPhine3GS以前の解像度の場合は仮想的に４倍にする　
	  if (width <= 320 || height <= 320) {
		width *= 2;
		height *= 2;
		scale_factor_ = 2.0f;
	  }
      else if (width > 1500 || height > 1500) {
          // for new ipad
          width /= 2;
          height /= 2;
          scale_factor_ = 1.0f;
      }
               
	  drawframe_app::app_instance().tap_device_scale_factor_ = scale_factor_;
	  drawframe_app::app_instance().gr_.initialize(width, height, scale_factor_);
    }
  
    void render_control_ios::render(double timestamp) {

	  // デバイスの回転をワンテンポ遅らせる
	  if (orientation_countdown_ != 0) {
		if (--orientation_countdown_ == 0) {
		  do_rotate();
		}
	  }

	  //glDepthFunc(GL_LESS);
	  //glDepthRangef(0.0f, 1.0f);
	  //glDepthMask(1);
	  drawframe_app::app_instance().draw_frame_hook(timestamp);
    }

	void render_control_ios::do_rotate() {
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

  } // mac
}}// ts::appfw



namespace ts { namespace appfw { namespace gl {
	  appfw::gl::render_control::ptr appfw::gl::render_control_factory::create() {
		return new ts::appfw::mac::render_control_ios;
	  }
	}
  }
}



