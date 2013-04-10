// -*-tab-width:4-*-
//
// boost::timerの改良版
//

#ifndef TS_UTIL_TIMER_HPP
#define TS_UTIL_TIMER_HPP

#include "../Common/Common.h"

#if defined(TS_PLATFORM_WIN32)
#include "../PlatformDependent/Timer/timer_win32.hpp"
#endif
#if defined(TS_PLATFORM_POSIX)
#include "../PlatformDependent/Timer/timer_posix.hpp"
#endif

#endif
