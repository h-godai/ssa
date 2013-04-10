/* -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_COMMON_HPP
#define TS_APPGW_COMMON_HPP

#include "libTS/Common/Common.h"


#include <math.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <deque>
#include <stdexcept>
//#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#if defined(__APPLE__) 
#if defined(GLUT)
#include <GLUT/glut.h>
#else
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#endif
#else
#if defined(__linux__) || defined(GLUT)
#include <GL/gl.h>
#define glOrthof glOrtho
#endif
#endif

#include "libTS/Util/Logger.h"
#include "libTS/math/geometric.hpp"
#include "../exception.hpp"


namespace ts {
namespace appfw {
namespace gl {
  using boost::shared_ptr;
  typedef boost::mutex::scoped_lock scoped_lock;


  struct glfw_exception : appfw_exception {
	glfw_exception(const char* const & msg) : appfw_exception(msg)
	{}
	virtual ~glfw_exception() throw() {}
  };
  

}}} // ts::appfw::gl
#endif
