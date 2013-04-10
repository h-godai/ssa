//
// Windows専用コード
//
#pragma once

#if defined (_MSC_VER)

#include <iostream>
#include <streambuf>
#include <cstring>


namespace ngs {

	class nnStreambuf : public std::streambuf {
	public:
		virtual int_type overflow(int_type c = EOF) {
			if(c != EOF)
			{
				char buf[] = { c, '\0' };
				OutputDebugString(buf);
			}
			return c;
		}
	};

	class Os {
		MMRESULT period_handle;
		nnStreambuf dbgstream;
		std::streambuf *default_stream;
	public:
		Os() : period_handle(timeBeginPeriod(1)) {
			_set_error_mode(_OUT_TO_MSGBOX);
			default_stream = std::cout.rdbuf(&dbgstream);
#ifdef _DEBUG
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		}
		~Os() {
			timeEndPeriod(period_handle);
			std::cout.rdbuf(default_stream);
		}
	};
	
	/* 画面更新がモニタと同期可能か調べる(glutの初期化後に呼び出す事) */
	bool OsIsVsyncSwap(void) {
		bool res;
		const char* ext = (const char *)glGetString(GL_EXTENSIONS);
		res = std::strstr(ext, "WGL_EXT_swap_control") ? true : false;
#ifdef DEBUG
		if(res)
		{
			cout << "WGL_EXT_swap_control" << endl;
		}
#endif
		return res;
	}

	bool OsToggleVsyncSwap(int sync) {
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
		bool res = FALSE;

		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
		if(wglSwapIntervalEXT)
		{
			res = wglSwapIntervalEXT(sync);
		}
		return res;
	}
}

#endif
