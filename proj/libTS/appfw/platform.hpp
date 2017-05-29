#ifndef TS_APPFW_GL_PLATFORM_HPP
#define TS_APPFW_GL_PLATFORM_HPP

#include "libTS/Common/Common.h"

#if defined(GLUT)
#include "ext_glut/render_control.hpp"
#else

#if defined(TS_PLATFORM_LINUX)
#include "ext_linux/render_control.hpp"
#endif

#if defined(TS_PLATFORM_MAC)
#include "ext_mac/render_control.hpp"
#endif

#if defined(TS_PALTFORM_WIN32)
#include "ext_win32/render_control.hpp"
#endif
#endif

#endif

