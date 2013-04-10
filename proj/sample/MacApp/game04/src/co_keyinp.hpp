//
// キー入力
//
#pragma once

#include <vector>
#include <iostream>

namespace ngs {

enum {
	ASCII_ENTER = 13,																	// リターンキー
	ASCII_BS		= 8,																	// BSキー
	ASCII_DEL		= 127,																// DELキー
	ASCII_ESC		= 27,																	// ESCキー
};

enum {
	INP_KEY_F1		  = GLUT_KEY_F1 + 0x80,
	INP_KEY_F2		  = GLUT_KEY_F2 + 0x80,
	INP_KEY_F3		  = GLUT_KEY_F3 + 0x80,
	INP_KEY_F4		  = GLUT_KEY_F4 + 0x80,
	INP_KEY_F5		  = GLUT_KEY_F5 + 0x80,
	INP_KEY_F6		  = GLUT_KEY_F6 + 0x80,
	INP_KEY_F7		  = GLUT_KEY_F7 + 0x80,
	INP_KEY_F8		  = GLUT_KEY_F8 + 0x80,
	INP_KEY_F9		  = GLUT_KEY_F9 + 0x80,
	INP_KEY_F10		  = GLUT_KEY_F10 + 0x80,
	INP_KEY_F11		  = GLUT_KEY_F11 + 0x80,
	INP_KEY_F12		  = GLUT_KEY_F12 + 0x80,
	INP_KEY_UP		  = GLUT_KEY_UP + 0x80,
	INP_KEY_RIGHT	  = GLUT_KEY_RIGHT + 0x80,
	INP_KEY_DOWN	  = GLUT_KEY_DOWN + 0x80,
	INP_KEY_PAGE_UP	  = GLUT_KEY_PAGE_UP + 0x80,
	INP_KEY_PAGE_DOWN = GLUT_KEY_PAGE_DOWN + 0x80,
	INP_KEY_HOME	  = GLUT_KEY_HOME + 0x80,
	INP_KEY_END		  = GLUT_KEY_END + 0x80,
	INP_KEY_INSERT	  = GLUT_KEY_INSERT + 0x80,
};

struct keyrec {
	u_char key;
	bool push;
};

const size_t KEY_INPUT_KINDNUM = 256;

class Keyinp {
	static int que_page;
	static std::vector<keyrec> inp_que[2];
	static u_char last_key[2];
	static std::vector<int> key_press;
	// FIXME:現時点ではglutとのハンドリングの為こう書かざるを得ない

	static void pushCallback(u_char key, int x, int y) {
		// キーボードから押せないであろうキーの入力は無視する
		if(key < 0x80)
		{
			keyrec rec;
			rec.key = key;
			rec.push = true;
			inp_que[que_page].push_back(rec);
			last_key[que_page] = key;
			key_press[key] = 1;
		}
	}

	static void pullCallback(u_char key, int x, int y) {
		// キーボードから押せないであろうキーの入力は無視する
		if(key < 0x80)
		{
			keyrec rec;
			rec.key = key;
			rec.push = false;
			inp_que[que_page].push_back(rec);
			key_press[key] = 0;
		}
	}

	static void exPushCallback(int key, int x, int y) {
		keyrec rec;
		rec.key = key + 0x80;;
		rec.push = true;
		inp_que[que_page].push_back(rec);
		last_key[que_page] = key + 0x80;
		key_press[key + 0x80] = 1;
	}

	static void exPullCallback(int key, int x, int y) {
		keyrec rec;
		rec.key = key + 0x80;;
		rec.push = false;
		inp_que[que_page].push_back(rec);
		key_press[key + 0x80] = 0;
	}
		
	Keyinp() {
		std::cout << "Keyinp()" << std::endl;
		last_key[0] = '\0';
		last_key[1] = '\0';

		glutKeyboardFunc(pushCallback);
		glutKeyboardUpFunc(pullCallback);
		glutSpecialFunc(exPushCallback);
		glutSpecialUpFunc(exPullCallback);
	}

public:
	static Keyinp& instance() {
		static Keyinp keyinp;
		return keyinp;
	}
		
	~Keyinp() {
		glutKeyboardFunc(NULL);
		glutKeyboardUpFunc(NULL);
		glutSpecialFunc(NULL);
		glutSpecialUpFunc(NULL);
	}

	void repeat(bool aRep) {
		glutIgnoreKeyRepeat(aRep ? 0 : 1);
	}

	void update() {
		que_page ^= 1;
		inp_que[que_page].clear();
		last_key[que_page] = '\0';
	}

	u_char get() {
		return last_key[que_page ^ 1];
	}
	bool press(u_char key) {
		return key_press[key] ? true : false;
	}
	bool push(u_char key) {
		bool res = false;
		for(std::vector<keyrec>::iterator it = inp_que[que_page ^ 1].begin(); it != inp_que[que_page ^ 1].end(); ++it)
		{
			if(it->push && (it->key == key))
			{
				res = true;
				break;
			}
		}
		return res;
	}
	bool pull(u_char key) {
		bool res = false;
		for(std::vector<keyrec>::iterator it = inp_que[que_page ^ 1].begin(); it != inp_que[que_page ^ 1].end(); ++it)
		{
			if(!it->push && (it->key == key))
			{
				res = true;
				break;
			}
		}
		return res;
	}
};

int Keyinp::que_page;
std::vector<keyrec> Keyinp::inp_que[2];
u_char Keyinp::last_key[2];
std::vector<int> Keyinp::key_press(KEY_INPUT_KINDNUM);
// FIXME:現時点ではglutとのハンドリングの為こう書かざるを得ない

}
