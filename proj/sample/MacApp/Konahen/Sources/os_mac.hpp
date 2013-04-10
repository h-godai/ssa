//
// OSX専用コード
//
#pragma once

#if defined (__APPLE__)

#include <OpenGL/OpenGL.h>
#include <iostream>
#include <streambuf>

namespace ngs {

	class Os {
	public:
		Os() {
			std::cout << "Welcome to my application." << std::endl;
		}
	};

	bool OsIsVsyncSwap() {
		/* Appleの資料を信じるならば、どのMacでも対応している */
		return true;
	}

	bool OsToggleVsyncSwap(int sync)
	{
		CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &sync);
		return true;
	}
}

#endif

