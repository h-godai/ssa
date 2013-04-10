/* -*-tab-width:4-*-
// timer for windows
*/


#include<windows.h>

namespace ts { 
  class timer {
  public:
	timer() {
	  QueryPerformanceCounter(&startTime);
	  QueryPerformanceFrequency(&frequency);
	}
	void restart() {
	  QueryPerformanceCounter(&startTime);
	}
 
	//経過時間を秒で返す
	double elapsed() const {
	  LARGE_INTEGER stopTime;
	  QueryPerformanceCounter(&stopTime);
	  return static_cast<double>(stopTime.QuadPart - startTime.QuadPart)/static_cast<double>(frequency.QuadPart);
	}
  private:
	LARGE_INTEGER startTime;
	LARGE_INTEGER frequency;        // ticks per second
  }; // Timerr

}

