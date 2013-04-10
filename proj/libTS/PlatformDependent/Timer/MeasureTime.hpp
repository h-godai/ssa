/**

	@File
	@Author	�r�c���� <godai@techarts.co.jp>
	*/

#ifndef TSL_PLATFORMDEPENDENT_MEASURETIME_H
#define TSL_PLATFORMDEPENDENT_MEASURETIME_H

#include <assert.h>
#include "../../Common/Common.h"

namespace ts {

	const uint32_t MEASURE_TICK_MICROSEC = 1000000;  // �P�ʂ̓ʕb�@ �ő�72��(uint32_t) 58���N(uint64_t)
	const uint32_t MEASURE_TICK_MILLISEC = 1000;     // �P�ʂ̓~���b �ő�49��(uint32_t) 5��8000���N(uint64_t)

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

	/// �v�����J�n����
	void start() {
		m_start = getCPUCounter();
		m_last = m_start;
		m_measureCount++;
	}

	/// �v���J�n������̌o�ߎ��Ԃ𓾂�
	uint64_t lap() {
		int64_t now = getCPUCounter();
		m_last = now;
		return (now - m_start);
	}

	/// �O��v��������̑��Ύ��Ԃ𓾂�
	uint64_t rlap() {
		int64_t now = getCPUCounter();
		int64_t rlap = now - m_last;
		m_last = now;
		return rlap;
	}

	/// �v�����ꎞ���~����
	uint64_t pause() {
		int64_t now = getCPUCounter();
		m_last = now;
		return (now - m_start);
	}

	/// �v���𒆎~����
	uint64_t stop() {
		int64_t now = getCPUCounter();
		m_last = now;
		int64_t t = (now - m_start);
		m_totalTime += t;
		return t;
	}

	/// �v�����ĊJ����
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



