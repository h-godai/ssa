/* GLCommon_h



*/


#ifndef TS_GLCommon_h
#define TS_GLCommon_h

#include "libTS/Common/Common.h"
#include <assert.h>
#include <math.h>
#ifndef STFAFX_H
#include <string>
#include <map>
#endif

#ifdef TS_PLATFORM_WIN32
#pragma warning (disable : 4201)
#endif

#include "libTS/Image/Color.hpp"

namespace ts {

  struct GObjException : public std::exception {
    GObjException(const char* const& msg)
      : errorMessage(msg)
    {}
    virtual ~GObjException() throw() {}
    std::string errorMessage;
  };




    enum RenderingModeType {
        RENDERINGMODE_TEXT,
        RENDERINGMODE_GL,
        RENDERINGMODE_DIRECTX,
        RENDERINGMODE_VTX,
        RENDERINGMODE_2D,
        RENDERINGMODE_CTREE
    };



} // 

#endif
