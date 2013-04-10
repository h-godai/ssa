

#include "stdafx.h"
#include "MeasureTime.hpp"
#ifdef TS_PLATFORM_POSIX
#include <sys/time.h>
#endif

namespace ts {

	int64_t MeasureTime::m_cpuClock = 0;

    MeasureTime::MeasureTime() : 
		m_start(0), 
		m_last(0), 
		m_tickPermSec(DEFAULT_MEASURE_TICK/1000),
		m_totalTime(0),
		m_measureCount(0)
		{
		if (!valid()) {
			assert(0); // TODO: throwする
		}
		if (m_cpuClock == 0) {
			m_cpuClock = calcCPUclock();
		}
#ifdef WIN32
        // 使用するCPUコアを限定する
        SetThreadAffinityMask(GetCurrentThread(), 1);
#endif

	}

	int64_t MeasureTime::calcCPUclock() {
#ifdef WIN32
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
		return freq.QuadPart;
#endif
#ifdef TS_PLATFORM_POSIX
		return 1000000;
#endif
	}



	int64_t MeasureTime::getCPUCounter() {
#ifdef WIN32
      LARGE_INTEGER t;
      QueryPerformanceCounter(&t);
      return t.QuadPart;
#endif
#ifdef TS_PLATFORM_POSIX
	  struct timeval tv;
	  ::gettimeofday(&tv, 0);
	  return tv.tv_sec * 1000000LL + tv.tv_usec;
#endif
    }

  
	bool MeasureTime::valid() {
#ifdef WIN32
	  // CPUにRDTSCレジスタがあるか調べる
	  if (IsProcessorFeaturePresent(PF_RDTSC_INSTRUCTION_AVAILABLE) == TRUE) {
		return true;
	  }
	  else {
		return false;
	  }
#else
#ifdef TS_PLATFORM_POSIX
	  return true;
#else
	  return false;
#endif
#endif
	}


}
