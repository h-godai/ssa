

#ifndef TS_THREAD_SOLARIS_HPP
#define TS_THREAD_SOLARIS_HPP

#include "base/ThreadBase.hpp"

#include <thread.h>

namespace tsl {

  class thread_solaris : public ThreadBase {
  public:
    thread_solaris(tThreadMain, void*);
    virtual ~thread_solaris();
    virtual bool run();
    virtual void sleep(int msec); // sleep

  private:
    thread_t m_thread;
  };

}

#endif
