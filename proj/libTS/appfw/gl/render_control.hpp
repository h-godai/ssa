/* -*-tab-width:4-*-
 *  render_control.hpp
 *  RiseOut
 *
 *  Created by IKEDA KOHEI on 11/05/02.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TS_APPFW_GL_RENDER_CONTROL_HPP
#define TS_APPFW_GL_RENDER_CONTROL_HPP

#include <stdint.h>

namespace ts {
namespace appfw {
namespace gl {

  // render control
  struct render_control {
    typedef render_control* ptr;

	// デバイスの向き
	enum device_orientation {
	  unknown_orientation
	  , orientation_portrait               // angle=0
	  , orientation_portrait_upside_down   // angle=180
	  , orientation_landscape_left         // angle=270
	  , orientation_landscape_right        // angle=90
	  , orientation_faceup                 // angle=0
	  , orientation_facedown               // angle=0
	};
	
    virtual void initialize(uint32_t width, uint32_t height, float scf=1.0f) = 0;
    virtual void render(double time_stamp) = 0;
    virtual void update_animation(double time_stamp) = 0;
    virtual void on_rotate(device_orientation orientation) = 0;
	virtual void enable_animation(bool enable) = 0;
    virtual ~render_control() throw() {}
  };

  // create render control object
  struct render_control_factory {
    static render_control::ptr create();
  };


}}} // ts::appfw::ios
#endif


