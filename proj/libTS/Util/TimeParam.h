// -*-tab-width:4-*-

#ifndef TS_TIME_PARAM_H
#define TS_TIME_PARAM_H

#include <memory.h>
#include "../Common/StdTypes.h"

struct TS_TimeParam_ {
  uint32_t year_;  // 
  uint32_t month_; // 1-12
  uint32_t day_;   // 1-31
  uint32_t hour_;  // 0-23
  uint32_t min_;   // 0-59
  uint32_t sec_;   // 0-59
  uint32_t usec_;  // 000000-999999 μ秒
#ifdef __cplusplus
  TS_TimeParam_() : year_(0), month_(1), day_(1), hour_(0), min_(0), sec_(0), usec_(0) {}
  TS_TimeParam_(uint32_t y, uint32_t m, uint32_t d, uint32_t h, uint32_t mm, uint32_t s, uint32_t u=0)
    : year_(y)
    , month_(m)
    , day_(d)
    , hour_(h)
    , min_(mm)
    , sec_(s)
    , usec_(u)
  {}
  TS_TimeParam_(const TS_TimeParam_& tp) {
    memcpy((void*)this, (const void*)&tp, sizeof(TS_TimeParam_));
  }
#endif
};


typedef struct TS_TimeParam_ TS_TimeParam;
#endif


