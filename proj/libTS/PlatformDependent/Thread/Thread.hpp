// Thread.hpp


#ifndef PLATFORMDEPENDENT_THREAD_HPP
#define PLATFORMDEPENDENT_THREAD_HPP

#include "IThread.hpp"


#ifdef WIN32
#include "ext_win32/pthread_win32.hpp"
namespace ts {
  typedef mutex_win32 Mutex;
  typedef condition_win32 Condition;
  typedef pthread_win32 Thread;
}
#endif


namespace ts {

  struct ScopedLock {
    Mutex& mutex;
    ScopedLock(Mutex& m) : mutex(m) {
      mutex.lock();
    }
    ~ScopedLock() {
      mutex.unlock();
    }
  };

}



#endif
