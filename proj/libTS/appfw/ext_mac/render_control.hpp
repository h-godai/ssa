/* -*-tab-width:4-*-
 *  render_control.hpp
 *  RiseOut
 *
 *  Created by IKEDA KOHEI on 11/05/02.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TS_APPFW_IOS_RENDER_CONTROL_HPP
#define TS_APPFW_IOS_RENDER_CONTROL_HPP

#include "ios_device.hpp"

#include <stdint.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "../gl/render_control.hpp"
#include "libTS/math/pos.hpp"
#include "libTS/math/size.hpp"

namespace ts {
namespace appfw {
namespace mac {

  // render control
  struct render_control_ios : ts::appfw::gl::render_control {
    GLuint framebuffer_;
    GLuint renderbuffer_;
    GLuint depthRenderbuffer;
    double time_stamp_;
	float scale_factor_;
	bool using_depth_buffer_;
	device_orientation orientation_;
	uint32_t orientation_countdown_;
	static const uint32_t rotate_delay_frame_ = 30;
    
    render_control_ios();
    void initialize(uint32_t width, uint32_t height, float scf=1.0f);
	void render(double timestamp) ;
    
    virtual void update_animation(double time_stamp) {
      //printf("update_animation %f\n", time_stamp);
    }

	virtual void enable_animation(bool enable) {
	  orientation_countdown_ = 0;
	}
    virtual void on_rotate(device_orientation orientation) {
      if (orientation_ != orientation) {
		orientation_ = orientation;
		orientation_countdown_ = rotate_delay_frame_;
	  }
	}

	void do_rotate();


	static void render_string(const std::string& str, math::pos2_f pos = math::pos2_f()) {
	}
	static void render_string(const char* str, math::pos2_f pos = math::pos2_f()) {
	}
	static const math::size_f charactor_size() {
	  return math::size_f(9.0f,15.0f);
	}
	static math::size_f get_textsize(const std::string& txt, int fontsize) {
	  return math::size_f(0.0f,0.0f);
	}


 };
  
  
}

  namespace gl {

	typedef ts::appfw::mac::render_control_ios rc;
  }

}} // ts::appfw::ios
#endif

  
