//
// Windows専用コード
//
#pragma once

#if defined (_MSC_VER)

#include <iostream>
#include <string>
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
	MMRESULT period_handle_;
	nnStreambuf dbgstream_;
	std::streambuf *default_stream_;
	std::string path_;
public:
	Os() : period_handle_(timeBeginPeriod(1)), path_(".") {
		_set_error_mode(_OUT_TO_MSGBOX);
		default_stream_ = std::cout.rdbuf(&dbgstream_);
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	}
	~Os() {
		timeEndPeriod(period_handle_);
		std::cout.rdbuf(default_stream_);
	}

	const std::string& path() const { return path_; }

/* 画面更新がモニタと同期可能か調べる(glutの初期化後に呼び出す事) */
	bool isVsyncSwap() const {
		const char* ext = (const char *)glGetString(GL_EXTENSIONS);
		bool res = std::strstr(ext, "WGL_EXT_swap_control") ? true : false;
#ifdef _DEBUG
		if(res)
		{
			std::cout << "WGL_EXT_swap_control" << std::endl;
		}
#endif
		return res;
	}
		
	bool toggleVsyncSwap(int sync) const {
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );

		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
		return (wglSwapIntervalEXT) ? wglSwapIntervalEXT(sync) : false;
	}
};
	

}

#endif
