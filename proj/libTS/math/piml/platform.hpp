/********************************************************************** -*-tab-width:4-*-

    @brief Platform Independent Math Library
    @brief 環境に依存しない数値演算ライブラリ
    @author h.godai godai@techarts.co.jp
    @note  サポートプラットフォーム
    @note  Windows (x86,x64), iPhone(ARM), Android(x86), PS-VITA(ARM), PS3(PowerPC,cell)

****************************************************************************************/


#ifndef TS_PIML_PLATFORM_HPP
#define TS_PIML_PLATFORM_HPP


//コンパイラの識別 
#if defined(__GNUC__) && !defined(__SNC__)
// for gcc, g++
#include <stdint.h>

#define TS_GNU_COMPILERC

#elif defined(__SNC__)
// for Sony Compiler
#include <stdint.h>
#define TS_SNC_COMPILER

#elif defined(_MSC_VER)
// for windows
#include <stdint.h>

#define TS_MSVC_COMPILER

#elif defined(__APPLE__)
// for iOS
#include <cstdint>
#define TS_APPLE_COMPILER

#else

#error No Supported Compiler

#endif


// 環境の識別
namespace piml {

    enum EndianType {
        LittleEndian,
        BigEndian
    };

    enum PlatformType {
        Platform_Windows,
        Platform_PSvita,
        Platform_PS3,
        Platform_iOS,
        Platform_Android
    };

#if defined(WIN32) || defined(WIN64)
    const EndianType Endian = LittleEndian;
    const PlatformType Platform = Platform_Windows;
#elif defined(GA_PLATFORM_PSP2) || defined(VITA)
    const EndianType Endian = LittleEndian;
    const PlatformType Platform = Platform_PSvita;
#elif defined(GA_PLATFORM_PS3) || defined(PS3) || defined(CELL)
//    const EndianType Endian = LittleEndian;
    const EndianType Endian = BigEndian;
    const PlatformType Platform = Platform_PS3;
#elif defined(GA_PLATFORM_IOS) || defined(IOS) || defined(__APPLE__)
    const EndianType Endian = LittleEndian;
    const PlatformType Platform = Platform_iOS;
#elif defined(PLATFORM_ANDROID) || defined(ANDROID)
    const EndianType Endian = LittleEndian;
    const PlatformType Platform = Platform_Android;
#else
#error unknown platform type
#endif


} // namespace piml

#endif

