// -*-tab-width:4-*-
//  glview.hpp
//  LibTS application framework library
//
//  Created by IKEDA KOHEI on 2012/3/24
//  Copyright 2012 TECHNICAL ARTS h.godai rights reserved.
//
#pragma once
	//#import <UIKit/UIKit.h>
	//#import <OpenGLES/EAGL.h>
	//#import <QuartzCore/QuartzCore.h>
	//#import <OpenGLES/ES1/gl.h>
	//#import <OpenGLES/ES1/glext.h>
	//#import <AudioToolbox/AudioToolbox.h>
	//#import <AVFoundation/AVFoundation.h>

#define DATE_TIME_INLINE
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include "libTS/appfw/appfw.hpp"
#include "libTS/math/geometric.hpp"
#include "libTS/Util/timer.hpp"

namespace ts { namespace appfw {
	using ts::math::rect_i;
    namespace ogl {
      class render_control_ogl;
    }
	class glview {
	  ts::timer timer_;
	  int argc_;
	  char** argv_;
	  rect_i framerect_;
	  app_info::ptr appinfo_;
	  double music_time_;
	public:
	  glview(int argc, char* argv[], app_info::ptr info)
		: argc_(argc)
		, argv_(argv)
		, appinfo_(info)
		, music_time_(0.0)
	  {}
	  double timestamp_;

	  void initializeFrame(const rect_i& framerect) { framerect_ = framerect; }

	  ogl::render_control_ogl* render_control_;

	  void run();

	  void drawView(); //CADisplayLink*) displayLink;
	  void onKeyDown(int key);
	  void audio_hook();
	private:
		void glutproc();
	  void didRotate(int direction); // NSNotification*) notification;

	};

}} // namespace ts:appfw




