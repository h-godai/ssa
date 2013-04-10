#pragma once

namespace ts {
// timevalの機能拡張
  struct TimeVal : timeval {
    TimeVal(bool getnow=false) {
      if (getnow) getNow();
    }
    TimeVal(long long usec) {
      tv_sec = usec / 1000000LL;
      tv_usec = static_cast<unsigned long long>(usec) % 1000000LL;
    }

    TimeVal& getNow() {
      if (snc_gettimeofday(this, NULL) != 0) {
	PANIC("gettimeofdat failed");
      }
      return *this;
    }

    // マイクロ秒をuint64で返す
    long long getusecond() const {
      return tv_sec * 1000000ULL + tv_usec;
    }

    bool operator < (const timeval& tv) {
      if (tv_sec == tv.tv_sec) {
	return tv_usec < tv.tv_usec;
      }
      else {
	return tv_sec < tv.tv_sec;
      }
    }
    long long operator - (const timeval& tv) {
      int sec = tv_sec - tv.tv_sec;
      unsigned int usec = 0;
      if (tv.tv_usec > tv_usec) {
	sec -= 1;
	usec = 1000000 + tv_usec - tv.tv_usec;
      }
      else {
	usec = tv_usec - tv.tv_usec;
      }
      return sec * 1000000LL + usec;
    }
    void operator += (int microsecond) {
      tv_usec += microsecond;
      if (tv_usec > 999999) {
	tv_usec -= 1000000;
	tv_sec += 1;
      }
    }
  };

  struct NanoSleep {

    static void wait(int millisecond) {
      TimeVal tv;
      tv.tv_sec = millisecond / 1000;
      tv.tv_usec = (millisecond % 1000) * 1000;
      wait(tv);
    }
    static void wait(const TimeVal& tv) {
      struct timespec req;
      struct timespec rem;
      req.tv_sec = tv.tv_sec;
      req.tv_nsec = tv.tv_usec * 1000;
      for (;;) {
	if (snc_nanosleep(&req, &rem) == 0) {
	  return;
	}
	switch(errno) {
	case EINTR: // シグナルによる割り込み
	  req = rem;
	  //	  return;
	  break;
	case EFAULT:
	case EINVAL:
	default:
	  WARNING_LOG("nanosleep result error(%d)", errno);
	  return;
	}
      }
    }
  };

}
