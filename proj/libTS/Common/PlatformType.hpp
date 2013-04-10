
#ifndef TS_PLATFORMTYPE_HPP
#define TS_PLATFORMTYPE_HPP

#include "Common.h"

// プラットフォームのタイプを設定する
namespace ts {
  enum PlatformType {
    PLATFORM_UNKNOWN
    , PLATFORM_WIN32
    , PLATFORM_WIN64
    , PLATFORM_LINUX
    , PLATFORM_MAC
    , PLATFORM_PS3
  };
#if defined(TS_PLATFORM_WIN32)
  const PlatformType CurrentPlatformType = PLATFORM_WIN32;
#else
#if defined(TS_PLATFORM_LINUX)
  const PlatformType CurrentPlatformType = PLATFORM_LINUX;
#else
#if defined(TS_PLATFORM_MAC) 
  const PlatformType CurrentPlatformType = PLATFORM_MAC;
#else
  const PlatformType CurrentPlatformType = PLATFORM_UNKNOWN;
#endif
#endif
#endif

} // namespace ts

#endif // TS_PLATFORMTYPE_HPP
