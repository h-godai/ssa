
#if defined (_MSC_VER)
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#include <windows.h>
#endif


#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <glut.h>																		// Winでは自分のローカルな場所にヘッダを置いた
#include <glext.h>
#endif

#if defined (_MSC_VER)
#include <memory>
#else
#include <tr1/memory>
#endif

#include "co_defines.hpp"
#include "co_mouse.hpp"
#include "co_keyinp.hpp"
#include "os_mac.hpp"
#include "os_win.hpp"
#include "nn_app.hpp"


using namespace std;
using namespace std::tr1;


const int FRAME_RATE			= 60;											// １秒あたりのループ回数
const int UPDATE_INTERVAL = 1000 / FRAME_RATE;			// 更新間隔(ms)
const int window_width		= 512;
const int window_height		= 512;

static GLuint display_list;
static GLuint display_page;
static int		intarval_time;
static int		width_, height_;
static bool		muiti_touch = false;


static void timerCallback(int value)
{
	glutTimerFunc(intarval_time, timerCallback, 0);
	glutPostRedisplay();
}

static void resizeCallback(int w, int h)
{
	width_ = w;
	height_ = h;
	
	// ngs::App::instance().resize(w, h);
	// ngs::App::instance().resizeFit(w, h);
	ngs::App::instance().resizeKeepAspect(w, h);
}

static void displayCallback()
{
	ngs::Keyinp::instance().update();
	ngs::Mouse::instance().update();

	ngs::App::instance().step();

	glNewList(display_list + display_page, GL_COMPILE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ngs::App::instance().draw();

	glEndList();
	glCallList(display_list + display_page);
	display_page ^= 1;
	glutSwapBuffers();
	
	unsigned char key_inp = ngs::Keyinp::instance().get();
	if((key_inp == 0x11) || (key_inp == 0x17))
	{
		exit(0);
	}
	else
	if(key_inp == ngs::INP_KEY_F5)
	{
		cout << "Soft Reset" << endl;
		ngs::App::instance().reset();
		resizeCallback(width_, height_);								// サイズ変更を実行しておく
	}
}

static void firstDisplayCallBack()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();

	glutDisplayFunc(displayCallback);
}


static void mouseCallBack(int button, int state, int x, int y)
{
	enum ngs::MOUSE_BUTTON btn;
	if(button == GLUT_LEFT_BUTTON)
	{
		btn = ngs::MOUSE_LEFT;
	}
	else
	if(button == GLUT_MIDDLE_BUTTON)
	{
		btn = ngs::MOUSE_MIDDLE;
	}
	else
	{
		// btn = ngs::MOUSE_RIGHT;
		btn = ngs::MOUSE_MULTI_TOUCH;										// iPhoneのマルチタッチを真似る
		muiti_touch = (state == GLUT_DOWN);
	}

	enum ngs::MOUSE_STATE st;
	if(state == GLUT_DOWN)
	{
		st = ngs::MOUSE_PUSH;
	}
	else
	{
		st = ngs::MOUSE_PULL;
	}

	ngs::Mouse::instance().buttonCallBack(btn, st);
	if(muiti_touch)
	{
		ngs::Mouse::instance().cursorCallBack(1, width_ - x, height_ - y);
	}
}

static void motionCallback(int x, int y)
{
	ngs::Mouse::instance().cursorCallBack(x, y);
	if(muiti_touch)
	{
		ngs::Mouse::instance().cursorCallBack(1, width_ - x, height_ - y);
	}
}


static void mainFin()
{
	// 終了時に実行
	cout << "Program fin!" << endl;
	ngs::App::instance().close();											// FTGLでシングルトンを使っている為の措置
}

int main(int argc, char **argv)
{
	ngs::Os os;																				// TIPS:OS依存のコードが実行されている

	cout << "Program start!" << endl;
	
	glutInit(&argc, argv);

	int x = -1;
	int y = -1;
	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);

	if(w > 0)
	{
		x = (w - (window_width + 8)) / 2;								// 若干のりしろを付けておく
		if(x < 0) x = -1;
	}
	if(h > 0)
	{
		y = (h - (window_height + 16)) / 2;							// 若干のりしろを付けておく
		if(y < 0) y = -1;
	}

	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(x, y);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("KONAHEN");

	cout << "OpenGL ver:" << glGetString(GL_VERSION) << endl;

	{
#if 0
#if defined (__APPLE__)
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char str[PATH_MAX];
		CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)str, PATH_MAX);
		CFRelease(resourcesURL);
		string path = string(str);
		cout << path << endl;
#else
		string path = ".";
#endif
#endif
	
		ngs::App::instance().initialize(window_width, window_height, os.path());
		ngs::App::instance().reset();
	}

	bool res = os.isVsyncSwap() && os.toggleVsyncSwap(1);
	intarval_time = res ? UPDATE_INTERVAL - 1 : UPDATE_INTERVAL;

	ngs::Mouse::instance();
	// FIXME:glutCreateWindow()された後じゃないと動かない
	
	glutDisplayFunc(firstDisplayCallBack);
	glutReshapeFunc(resizeCallback);
	glutTimerFunc(intarval_time, timerCallback, 0);
	atexit(mainFin);

	glutMouseFunc(mouseCallBack);
	glutMotionFunc(motionCallback);
	glutPassiveMotionFunc(motionCallback);

	glutMainLoop();

	return 0;
}
