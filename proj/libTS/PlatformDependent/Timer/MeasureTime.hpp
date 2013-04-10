/**

	@File
	@Author	池田公平 <godai@techarts.co.jp>
	*/

#ifndef TSL_PLATFORMDEPENDENT_MEASURETIME_H
#define TSL_PLATFORMDEPENDENT_MEASURETIME_H

#include <assert.h>
#include "../../Common/Common.h"

namespace ts {

	const uint32_t MEASURE_TICK_MICROSEC = 1000000;  // 単位はμ秒　 最大72分(uint32_t) 58万年(uint64_t)
	const uint32_t MEASURE_TICK_MILLISEC = 1000;     // 単位はミリ秒 最大49日(uint32_t) 5億8000万年(uint64_t)

	const uint32_t DEFAULT_MEASURE_TICK = MEASURE_TICK_MICROSEC;
	

class MeasureTime {
public:
	MeasureTime();
	~MeasureTime() {}

	static int64_t calcCPUclock();
	static int64_t calcCPUclockPerMillisecond();

	void setTick(uint32_t tick) {
		m_tickPermSec = tick / 1000;
	}

	/// 計測を開始する
	void start() {
		m_start = getCPUCounter();
		m_last = m_start;
		m_measureCount++;
	}

	/// 計測開始時からの経過時間を得る
	uint64_t lap() {
		int64_t now = getCPUCounter();
		m_last = now;
		return (now - m_start);
	}

	/// 前回計測時からの相対時間を得る
	uint64_t rlap() {
		int64_t now = getCPUCounter();
		int64_t rlap = now - m_last;
		m_last = now;
		return rlap;
	}

	/// 計測を一時中止する
	uint64_t pause() {
		int64_t now = getCPUCounter();
		m_last = now;
		return (now - m_start);
	}

	/// 計測を中止する
	uint64_t stop() {
		int64_t now = getCPUCounter();
		m_last = now;
		int64_t t = (now - m_start);
		m_totalTime += t;
		return t;
	}

	/// 計測を再開する
	void restart() {
		int64_t now = getCPUCounter();
		int64_t pasttime = now - m_last;
		m_start += pasttime;
		m_last = now;
	}


	static bool valid();
	static int64_t getCPUCounter();

    double toSecond(int64_t t) const {
		return t / static_cast<double>(m_cpuClock);
    }


	double getTotalTime() const {
      if (m_totalTime < 0) return 0.0;
		return m_totalTime / static_cast<double>(m_cpuClock);
	}
	uint32_t measureCount() const { return m_measureCount; }

private:
		static int64_t m_cpuClock;
		int64_t m_start;
		int64_t m_last;
		int64_t m_tickPermSec;
		int64_t m_totalTime;
		uint32_t m_measureCount;
};





} // namespace

#endif



