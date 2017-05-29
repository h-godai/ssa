// -*-tab-width:4-*-
//
//  glview.cpp
//
//  Created by IKEDA KOHEI on 2012/3/24
//  Copyright 2012 TECHNICAL ARTS h.godai rights reserved.
//
#include "../platform.hpp"
#include "glview.hpp"
#include "libTS/appfw/drawframe_app.hpp"
#include "libTS/Util/Logger.h"

namespace {
  using namespace ts::appfw;
  using namespace ts::appfw::in;

  ts::appfw::glview* glv = 0;
  bool firstresize = false;

#if 0
  static int slices = 16;
  static int stacks = 16;
#endif

  static void resize(int width, int height)
  {
    firstresize = true;
    TS_DEBUGLOG("resize to (%d,%d)\n", width, height);
#if 0
    const float ar = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
#endif
  }

  static void display(void)
  {
	if (glv != 0 && firstresize) {
	  glv->drawView();
	}
  }

#if 0
  static void displaytest() {

    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
//    glMatrixMode(GL_MODELVIEW);
 //   glLoadIdentity() ;
    glDisable(GL_DEPTH_TEST);

//    glScalef(320.0f,320.0f,320.0f);
//	glTranslated(400.0f,300.0f,0.0f);
    resize(800,600);

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);

    glPushMatrix();
	glTranslated(-2.4,1.2,-6);
	glRotated(60,1,0,0);
	glRotated(a,0,0,1);
	glutSolidSphere(1,slices,stacks);
    glPopMatrix();
#if 0
    glPushMatrix();
	glTranslated(0,1.2,-6);
	glRotated(60,1,0,0);
	glRotated(a,0,0,1);
	glutSolidCone(1,1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
	glTranslated(2.4,1.2,-6);
	glRotated(60,1,0,0);
	glRotated(a,0,0,1);
	glutSolidTorus(0.2,0.8,slices,stacks);
    glPopMatrix();

    glPushMatrix();
	glTranslated(-2.4,-1.2,-6);
	glRotated(60,1,0,0);
	glRotated(a,0,0,1);
	glutWireSphere(1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
	glTranslated(0,-1.2,-6);
	glRotated(60,1,0,0);
	glRotated(a,0,0,1);
	glutWireCone(1,1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
	glTranslated(2.4,-1.2,-6);
	glRotated(60,1,0,0);
	glRotated(a,0,0,1);
	glutWireTorus(0.2,0.8,slices,stacks);
    glPopMatrix();
    #endif

//    glutSwapBuffers();
  }
#endif

  static void key(unsigned char key, int x, int y)
  {
	TS_DEBUGLOG("glut:key(%x)\n", key);
	if (glv != 0 && firstresize) {
	  glv->onKeyDown(key);
	}
#if 0	
    switch (key)
	  {
	  case 27 :
	  case 'q':
		exit(0);
	  break;

	  case '+':
		slices++;
		stacks++;
		break;

	  case '-':
		if (slices>3 && stacks>3)
		  {
			slices--;
			stacks--;
		  }
		break;
	  }
#endif
    glutPostRedisplay();
  }

  static void idle(void)
  {
    glutPostRedisplay();
	if (glv != 0) {
	  glv->audio_hook();
	}
  }

  static void mousefunc(int button, int state, int x, int y) {
	//[ button ]
	//  GLUT_LEFT_BUTTON：「左ボタン」
	//  GLUT_MIDDLE_BUTTON：「中央ボタン」
	//　GLUT_RIGHT_BUTTON：「右ボタン」
	//[ state ]
	//　GLUT_DOWN：「ボタン」が「押された」
	//　GLUT_UP：「ボタン」が「離れた」[ button ]

	if (glv == 0) return;
	static bool btn[3] = { false, false, false };
	
	ts::math::pos2_f p(x, y);
	int device = 0;
	if (button == GLUT_RIGHT_BUTTON) device = 1;
	if (button == GLUT_MIDDLE_BUTTON) device = 2;
	switch (state) {
	case GLUT_DOWN:
	  if (!btn[device]) {
		app_framework::app_instance().notify_event(tap_ctrl(tap_ctrl::touch, p), device);
		btn[device] = true;
	  }
	  else {
		app_framework::app_instance().notify_event(tap_ctrl(tap_ctrl::move, p), device);
	  }
	  break;
	case GLUT_UP:
	  if (btn[device]) {
		app_framework::app_instance().notify_event(tap_ctrl(tap_ctrl::release, p), device);
		btn[device] = false;
	  }
	  break;
	default:
	  if (btn[device]) {
		app_framework::app_instance().notify_event(tap_ctrl(tap_ctrl::move, p), device);
	  }
	  break;
	}
  }

} // namespace

namespace ts { namespace appfw {

	using boost::posix_time::ptime;
	using boost::posix_time::microsec_clock;
//	using namespace ogl;

	void glview::glutproc() {

  	  glutInit(&argc_, argv_);
	  glutInitWindowSize(framerect_.width(), framerect_.height());
	  glutInitWindowPosition(10,10);
	  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	  glutCreateWindow("GLUT Shapes");

	  glutReshapeFunc(resize);
	  glutDisplayFunc(display);
	  glutKeyboardFunc(key);
	  glutMouseFunc(mousefunc);
	  glutIdleFunc(idle);

	  glv = this;

	  // initialize fremework context
	  render_control_ = new ogl::render_control_ogl;
	  render_control_->initialize(framerect_.width(), framerect_.height(), 1.0f);
	  app_framework::app_instance().initialize(appinfo_);
	  // soundの登録
	  //std::vector<std::string> soundfn;
	  //app_framework::app_instance().get_sound_list(soundfn);

	  // BGMの登録
	  //std::vector<std::string> musicfn;
	  //app_framework::app_instance().get_music_list(musicfn);



	  //[self drawView: nil];
	  //timestamp_ = CACurrentMediaTime();
	  glutMainLoop();
	}

	void glview::run() {
#ifdef __APPLE__
        glutproc();
#else
      boost::thread glutThread(&glview::glutproc, this);
      while (!drawframe_app::app_instance().is_stopped()) {
          sleep(1);
      }
#endif
      glv = 0;
	}


	void glview::didRotate(int direction) {
	  render_control_->on_rotate((gl::render_control::device_orientation)direction);
	}

	void glview::drawView() {
	  double now = timer_.elapsed();
	  double elapsed = now - timestamp_;
	  if (elapsed < 0.0 || elapsed > 60.0) {
		TS_DEBUGLOG("too long frame %fsec", elapsed);
		elapsed = 1.0;
	  }
	  timestamp_ = now;
	  render_control_->update_animation(elapsed);
	  render_control_->render(timestamp_);
// 	  displaytest();
      glutSwapBuffers();
//	  TS_DEBUGLOG("now:%f elapsed:%f\n", now, timer_.elapsed());
	  float fps = 1.0f / elapsed; // float(timer_.elapsed() - now);
	  app_framework::app_instance().set_fps(fps, static_cast<float>(timer_.elapsed() - now));
	}

	void glview::onKeyDown(int key) {
	  app_framework::app_instance().notify_event(in::key_ctrl(in::key_ctrl::pushed, key));
	}

	void glview::audio_hook() {
	  double now = timer_.elapsed();

	  std::vector<ts::appfw::sounds::audio_param> params;
	  app_framework::app_instance().audio_control_hook(params);
	  BOOST_FOREACH(ts::appfw::sounds::audio_param& param, params) {
		switch (param.type_) {
		case ts::appfw::sounds::sound_type:
		  //AudioServicesPlaySystemSound(soundIdList_[param.id_]);
		  break;
		case ts::appfw::sounds::music_type:
		  {
			//currentMusic_ = (AVAudioPlayer*)[music_array_ objectAtIndex:(NSUInteger)param.id_]; 
			//if ((NSString*)currentMusic_ == @"error") {
			//currentMusic_ = nil;
			//  break;
			//}
			if (param.cmd_ == ts::appfw::sounds::cmd_play) {
			  music_time_ = now;
			  //currentMusic_.numberOfLoops = -1; // 無限ループの指定
			  //currentMusic_.currentTime = 0.0f;
			  //[currentMusic_ play];
			}
			else {
			  //[currentMusic_ stop];
			  //currentMusic_ = nil;
			  music_time_ = 0.0;
			}
			// mBgm.numberOfLoops = -1;// 無限ループの指定
			// mBgm.numberOfLoops = 0;// １回ループの指定
			// mBgm.numberOfLoops = 1;// ２回ループの指定
		  }
		  break;
		}
	  }
	  if (music_time_ != 0.0) {
		app_framework::app_instance().set_current_music_time(now - music_time_);
	  }
	}


  }}  // namespace

