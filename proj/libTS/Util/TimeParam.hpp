// -*-tab-width:4-*-

#ifndef TS_TIME_PARAM_HPP
#define TS_TIME_PARAM_HPP

#include "../Common/StdTypes.h"
#include "TimeParam.h"

#ifdef __cplusplus

namespace ts {
  
  // マイクロ秒単位の時刻
  typedef uint64_t micro_time_t;
  
  struct MicroTime {
	micro_time_t mtime_;
	static const uint64_t MICRO_SEC = 1000000ULL;

	MicroTime(const MicroTime& t) : mtime_(t.mtime_) {}
	MicroTime(micro_time_t mt=0) : mtime_(mt) {}
	MicroTime(time_t t) : mtime_(static_cast<uint64_t>(t) * MICRO_SEC) {}
	MicroTime(double t) : mtime_(static_cast<micro_time_t>(t * MICRO_SEC)) {}
	MicroTime(uint32_t t) : mtime_(static_cast<micro_time_t>(t)) {}
	MicroTime(int t) : mtime_(static_cast<micro_time_t>(t)) {}

	operator micro_time_t() const { return mtime_; }
	operator double() const { return mtime_ / static_cast<double>(MICRO_SEC); }
	operator time_t() const { return time(); }

	uint32_t day() const { return static_cast<uint32_t>(mtime_ / (MICRO_SEC*3600*24)); }
	uint32_t hour24() const { return static_cast<uint32_t>(mtime_ / (MICRO_SEC*3600)) % 24; }
	uint32_t hour() const { return static_cast<uint32_t>(mtime_ / (MICRO_SEC*3600)); }
	uint32_t min() const { return static_cast<uint32_t>(mtime_  / (MICRO_SEC*60)) % 60; }
	uint32_t min_only() const { return static_cast<uint32_t>(mtime_  / (MICRO_SEC*60)); }
	uint32_t sec() const { return static_cast<uint32_t>(mtime_ / MICRO_SEC) % 60; }
	double   secf() const {return (mtime_ % (60*MICRO_SEC)) / static_cast<double>(MICRO_SEC); }
	uint32_t sec_only() const { return static_cast<uint32_t>(mtime_ / MICRO_SEC); }
	uint32_t usec() const { return static_cast<uint32_t>(mtime_ % MICRO_SEC); }
	uint32_t msec() const { return static_cast<uint32_t>(mtime_ % MICRO_SEC)/1000; }
	time_t time() const { return static_cast<time_t>(sec()); }

	// 文字列からの変換
	MicroTime(const char* utf8)
	{
	  const char* coron = strchr(utf8, ':');
	  if (coron != NULL) {
		int min = 0;
		double sec = 0.0;
		sscanf(utf8, "%d:%lf", &min, &sec);
		mtime_ = min * 60 * 1000000ULL + static_cast<micro_time_t>(sec * static_cast<double>(MICRO_SEC));
	  }
	  else {
		mtime_ = atof(utf8) * 1000000.0;
	  }
	}

	/// 評価
	bool empty() const { return mtime_ == 0; }

	/// 演算子
	MicroTime& operator += (const MicroTime& t) { mtime_ += t.mtime_; return *this; }
	MicroTime& operator -= (const MicroTime& t) { mtime_ -= t.mtime_; return *this; }
	bool operator > (const MicroTime& t) const { return mtime_ > t.mtime_; }
	bool operator == (const MicroTime& t) const { return mtime_ == t.mtime_; }
	bool operator < (const MicroTime& t) const { return mtime_ < t.mtime_; }
	MicroTime operator - (const MicroTime& t2) const { return mtime_ - t2.mtime_; }
	MicroTime operator + (const MicroTime& t2) const { return mtime_ + t2.mtime_; }

	/// 文字列生成
	// HH:MM:SS
	std::string toHMS() const {
	  char buf[64];
	  snprintf(buf, 64, "%02u:%02u:%02u", hour(), min(), sec());
	  return buf;
	}
	// MM:SS
	std::string toMS() const {
	  char buf[64];
	  snprintf(buf, 64, "%2.2u:%02u", min_only(), sec());
	  return buf;
	}
	// MM:SS.xxx
	std::string toMSp3() const {
	  char buf[64];
	  snprintf(buf, 64, "%2.2u:%2.2u.%03u", min_only(), sec(), msec());
	  return buf;
	}
	// SS.x
	std::string toSp1() const {
	  char buf[64];
	  snprintf(buf, 64, "%2.2u.%u", sec_only(), msec()/100);
	  return buf;
	}
	// SS.xxx
	std::string toSp3() const {
	  char buf[64];
	  snprintf(buf, 64, "%2.2u.%03u", sec_only(), msec());
	  return buf;
	}

  };

  
  struct TimeParam : TS_TimeParam_ {
	TimeParam() {}

	TimeParam(const TS_TimeParam& tp) 
	  : TS_TimeParam_(tp)
	{}
    
	TimeParam(time_t t) 
	{
	  struct tm* lt = localtime(&t);
	  year_ = lt->tm_year + 1900;
	  month_ = lt->tm_mon + 1;
	  day_ = lt->tm_mday;
	  hour_ = lt->tm_hour;
	  min_ = lt->tm_min;
	  sec_ = lt->tm_sec;
	  usec_ = 0;
	}

	// マイクロ秒のuint64_tからの変換
	TimeParam(MicroTime t)
	  : TS_TimeParam_(0,1,1
					  , t.hour()
					  , t.min()
					  , t.sec()
					  , t.usec())
	{}
	TimeParam(uint32_t h, uint32_t m, uint32_t s, uint32_t u) 
	  : TS_TimeParam_(1,0,0,h,m,s,u)
	{}
	TimeParam(uint32_t y, uint32_t m, uint32_t d, uint32_t h, uint32_t mm, uint32_t s, uint32_t u=0)
	  : TS_TimeParam_(y,m,d,h,mm,s,u)
	{}
	TimeParam(double t)
	  : TS_TimeParam_(0,1,1
					  , MicroTime(t).hour()
					  , MicroTime(t).min()
					  , MicroTime(t).sec()
					  , MicroTime(t).usec())
	{}
	operator micro_time_t() {
	  return ((hour_ * 3600ULL) + (min_ * 60ULL) + sec_) * 1000000ULL + usec_;
	}
	operator double() {
	  return ((hour_ * 3600) + (min_ * 60) + sec_) + (usec_ / 1000000.0);
	}
	operator TS_TimeParam() {
	  return static_cast<TS_TimeParam>(*this);
	}
	operator time_t() {
	  struct tm t;
	  t.tm_year = year_ - 1900;
	  t.tm_mon = month_ - 1;
	  t.tm_mday = day_;
	  t.tm_hour = hour_;
	  t.tm_min = min_;
	  t.tm_sec = sec_;
	  t.tm_isdst = -1;
#ifdef _BSD_SOURCE
	  t.tm_gmtoff = 0;
	  t.tm_zone = NULL;
#endif
	  return mktime(&t);
	}
  };

}

#endif // __cplusplus
#endif
