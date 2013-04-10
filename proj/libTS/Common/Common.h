/* Common.h   -*-tab-width:2-*-  */
/**
 * libTS Common  definition
 * Copyright (c) 2005-2010 TECHNICAL ARTS All Rights Reserved.
 *
 * @author h.godai godai@techarts.co.jp
 *
 * revision history
 * 2010-01-28 boostÇÃcstdintÇ…ëŒâû
 *
 */


#ifndef TS_COMMON_H
#define TS_COMMON_H

#ifdef _MSC_VER
// Microsoft VisualStudioÇÃèÍçá
#define NO_CSTDINT  // cstdintÇ™ñ≥Ç¢

// MFCÇÃ min / max É}ÉNÉçÇè¡Ç∑
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#endif // _MSC_VER

#ifdef _WIN32
// WindowsÇÃèÍçá
#define TS_PLATFORM_WIN32
#define TS_LITTLE_ENDIAN

#endif // _WIN32

#if defined(LINUX) || defined(__linux__)

#define TS_PLATFORM_LINUX
#define TS_LITTLE_ENDIAN
#define TS_PLATFORM_POSIX

#endif // LINUX

#ifdef __APPLE__
#undef TS_PLATFORM_LINUX
#define TS_PLATFORM_MAC
#define TS_LITTLE_ENDIAN
#define TS_PLATFORM_POSIX

//#define BOOST_SP_USE_PTHREADS
//#define BOOST_SP_DISABLE_THREADS 
//#define __thumb__


#endif // APPLE

#if defined(SPARC) ||defined(MIPS) || defined(USE_BIG_ENDIAN)
#undef TS_LITTLE_ENDIAN
#define TS_BIG_ENDIAN
#endif

#if defined(GA_PLATFORM_PS3) || defined(PLATFORM_PS3) || defined(POWER_PC)
#undef TS_LITTLE_ENDIAN
#define TS_BIG_ENDIAN
#endif

// define endian value
namespace ts {
  const int BIG_ENDIAN_VALUE = 1;
  const int LITTLE_ENDIAN_VALUE = 0;
#if defined(TS_BIG_ENDIAN)
  const int CURRENT_ENDIAN = BIG_ENDIAN_VALUE;
#else
  const int CURRENT_ENDIAN = LITTLE_ENDIAN_VALUE;
#endif
} // namespace ts

#if defined(NO_USE_BOOST)
#define TS_NO_BOOST 1
#else
#define TS_NO_BOOST 0
#endif



#if TS_NO_BOOST || !defined(__cplusplus)
// boostÉâÉCÉuÉâÉäÇégÇÌÇ»Ç¢èÍçá
#ifdef NO_CSTDINT
#include "StdTypes.h"
#else // NO_CSTDINF
#ifdef __cplusplus
#include <cstdint>
namespace ts {
  typedef float float32_t;
  typedef double float64_t;
}
#else
#include <stdint.h>
#endif // __cplusplus
#endif // NO_CSTDINF

#else // TS_NO_BOOST
// boostÉâÉCÉuÉâÉäÇégÇ§èÍçá
#if defined(__cplusplus)
#include <boost/cstdint.hpp>
namespace ts {
  using boost::uint8_t;
  using boost::uint16_t;
  using boost::uint32_t;
  using boost::uint64_t;
  using boost::int8_t;
  using boost::int16_t;
  using boost::int32_t;
  using boost::int64_t;
  typedef float float32_t;
  typedef double float64_t;
}


#endif //  __cplusplus
#endif // TS_NO_BOOST




#endif // TS_COMMON_H




