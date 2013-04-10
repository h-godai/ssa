//
// OSX専用コード
//
#pragma once

#if defined (__APPLE__)

#include <OpenGL/OpenGL.h>
#include <string>
#include <iostream>
#include <streambuf>
#include <CoreFoundation/CoreFoundation.h>

namespace ngs {

class Os {
	std::string path_;
public:
	Os() : path_(".") {
		std::cout << "Welcome to my application." << std::endl;

		CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char str[PATH_MAX];
		if(!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)str, PATH_MAX))
		{
			std::cout << "error CFURLGetFileSystemRepresentation()" << std::endl;
		}
		else
		{
			path_ = std::string(str);
		}
		CFRelease(resourcesURL);
	}

	const std::string& path() const { return path_; }

	bool isVsyncSwap() const
	{
		/* Appleの資料を信じるならば、どのMacでも対応している */
		return true;
	}

	bool toggleVsyncSwap(int sync) const
	{
		CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &sync);
		return true;
	}
};
}

#endif

