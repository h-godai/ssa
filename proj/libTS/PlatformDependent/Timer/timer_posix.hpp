/* -*-tab-width:4-*-
// timer for posix
*/

#include <time.h>
#include <sys/time.h>

namespace ts {

  class timer {
  public:
	timer()	{
	  restart();
	}
	void restart() {
	  gettimeofday(&startTime_, NULL);
	}

	//経過時間を秒で返す
	double elapsed() const {
	  timeval stopTime;
	  gettimeofday(&stopTime, NULL);
	  return (stopTime.tv_sec - startTime_.tv_sec) + ((stopTime.tv_usec - startTime_.tv_usec) * 1e-6);
	}
  private:
	timeval startTime_;
  };

}

