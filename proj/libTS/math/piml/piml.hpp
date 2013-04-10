/********************************************************************** -*-tab-width:4-*-

    @brief Platform Independent Math Library
    @brief 環境に依存しない数値演算ライブラリ
    @author h.godai godai@techarts.co.jp
    @note  サポートプラットフォーム
    @note  Windows (x86,x64), iPhone(ARM), Android(x86), PS-VITA(ARM), PS3(PowerPC,cell)

****************************************************************************************/

#ifndef __cplusplus
#error This file require C++ compiler
#endif

#ifndef TS_PIML_HPP
#define TS_PIML_HPP

#include "platform.hpp"

// GAライブラリ対応
#if defined(GA_PLATFORM_PS3) || defined(GA_PLATFORM_PSP2)
#include "ga/common.h"
#include "lib/boost_util.h"
#endif

#ifndef TS_PIML_ENDIAN_HPP
#include "endian.hpp"
#endif

#ifndef TS_PIML_SQRT_HPP
#include "sqrt.hpp"
#endif

#ifndef TS_PIML_TRIGONOMETRIC_HPP
#include "trigonometric.hpp"
#endif


#endif
