/* -*-tab-width:4-*-
 *  render_control.hpp
 *  RiseOut
 *
 *  Created by IKEDA KOHEI on 11/05/02.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TS_APPFW_LINUX_RENDER_CONTROL_HPP
#define TS_APPFW_LINUX_RENDER_CONTROL_HPP

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdint.h>
#include <string>
//#include "linux_device.hpp"
#include "../gl/render_control.hpp"
#include "libTS/math/pos.hpp"
#include "libTS/math/size.hpp"


namespace ts {
  namespace appfw {
	namespace ogl {

	  struct render_control_ogl : ts::appfw::gl::render_control {
		double time_stamp_;
		float scale_factor_;
		device_orientation orientation_;
		render_control_ogl();
		virtual ~render_control_ogl() throw() {}
		
		void initialize(uint32_t width, uint32_t height, float scf=1.0f);
		void render(double timestamp);
		void update_animation(double time_stamp);
		void enable_animation(bool enable);
		void on_rotate(device_orientation orientation);
		void do_rotate();


	  };

	  struct render_control_glut : render_control_ogl {
		virtual ~render_control_glut() throw() {}

		static void render_string(const std::string& str, math::pos2_f pos = math::pos2_f()) {
		  return render_string(str.c_str(), pos);
		}
		static void render_string(const char* str, math::pos2_f pos = math::pos2_f()) {
		  if (str == 0 || *str == 0) return;
		  glRasterPos3f(pos.x_, pos.y_, -1.0f);
		  const char* p = (const char*)str;
		  while (*p != 0) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p++); // GLUT_BITMAP_HELVETICA_18
		  }
		}
		static const math::size_f charactor_size() {
		  return math::size_f(9.0f,15.0f);
		}
		static math::size_f get_textsize(const std::string& txt, int fontsize) {
		  return math::size_f(charactor_size().width_ * txt.length(), charactor_size().height_);
		}


	  };
	}
	
	namespace gl {
	  typedef ts::appfw::ogl::render_control_glut rc;
	}
  }
} // ts::appfw::ios
#endif


