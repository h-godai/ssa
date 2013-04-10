

#ifndef TS_PTHREAD_LINUX_HPP
#define TS_PTHREAD_LINUX_HPP

#include "ThreadBase.hpp"

#ifdef __LINUX__

#include <pthread.h>

namespace ts {

  class pthread_linux : public ThreadBase {
  public:
    pthread_linux(tThreadMain, void*);
    virtual ~pthread_linux();
    virtual bool run();
    virtual void sleep(int msec); // sleep
  private:
    pthread_t m_thread;
  };

}

#endif
#endif
