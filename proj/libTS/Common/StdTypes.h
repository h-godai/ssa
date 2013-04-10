/* TSTypes.h -*-C++-*-

  Copyright 2005 TECHNICAL ARTS h.godai
*/

#ifndef TS_STDTYPES_H
#define TS_STDTYPES_H

#include "Common.h"

#include <limits.h>
#if CHAR_BIT!=8
#error char not 8bit
#endif

#if 0
// Common.hに集約


namespace ts {

#ifdef TS_PLATFORM_WIN32
  typedef unsigned char     uint8_t;
  typedef unsigned short    uint16;
  typedef unsigned long     uint32_t;
  typedef unsigned __int64  uint64_t;
  typedef signed char       int8_t;
  typedef signed short      int16;
  typedef signed long       int32_t;
  typedef signed __int64    int64_t;
  typedef float float32_t;
  typedef double float64_t;
#endif
#ifdef TS_PLATFORM_LINUX
  typedef signed   char    int8_t;
  typedef signed   short   int16;
  typedef signed   long    int32_t;
  typedef signed   long long int64_t;
  typedef unsigned char    uint8_t;
  typedef unsigned short   uint16;
  typedef unsigned long    uint32_t;
  typedef unsigned long long uint64_t;
  typedef float            float32;
  typedef double           float64;
#endif

}

#ifdef TS_PLATFORM_MAC
#include <stdint.h>
  typedef float            float32;
  typedef double           float64;
#endif


#endif

#endif
