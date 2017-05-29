/* threadproc.cpp  -*-C++-*-



*/
#ifdef __LINUX__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <sys/time.h>

#include "pthread_linux.h"


namespace tsl {

  //  pthread_cond_t  CondInitializer = PTHREAD_COND_INITIALIZER;
  //  pthread_mutex_t MutexInitializer = PTHREAD_MUTEX_INITIALIZER;

  typedef pthread_mutex_t Mutex_t;
  typedef pthread_cond_t Cond_t;

  class MutexLinux : public Mutex {
  public:
    Mutex_t mMutex;
    Cond_t  mCond;
    MutexLinux() {
	  initialize();
    }

    ~MutexLinux() {
	  finalize();
    }

    void initialize() {
//	  pthread_mutex_init(&mMutex, NULL);
//	  pthread_cond_init(&mCond, NULL);
	mCond	= PTHREAD_COND_INITIALIZER;
	mMutex	= PTHREAD_MUTEX_INITIALIZER;
    }

    void finalize() {
	  pthread_cond_destroy(&mCond);
	  pthread_mutex_destroy(&mMutex);
    }

    void lock() {
	  pthread_mutex_lock(&mMutex);
    }

    void unlock() {
	  pthread_mutex_unlock(&mMutex);
    }

    int cond_wait(int msec) {
      int result;
      lock();
      if (msec == 0) {
        result = mMutex.cond_wait();
      }
      else {
        struct timeval now;
        struct timespec t;
        if (!gettimeofday(&now, NULL)) {
	  now.tv_usec += msec * 1000;
	  if(now.tv_usec > 1000000) {
	    now.tv_sec += 1;
	    now.tv_usec -= 1000000;
	  }
	  t.tv_sec = now.tv_sec;
	  t.tv_nsec = now.tv_usec * 1000;
        }
        else {
	  // gettimeofdayに失敗した場合は秒単位
	  t.tv_sec = time(NULL) + (msec / 1000);
	  t.tv_nsec = 0;
        }
        result = mMutex.cond_timedwait(&t); 
      }
      unlock();
      return result;    
    }

    int cond_timedwait(const struct timespec *t) {
	  return pthread_cond_timedwait(&mCond, &mMutex, t);
    }
    
    int cond_signal() {
	  return pthread_cond_signal(&mCond);
    }

  };

  pthread_linux::pthread_linux() :
    mThreadStatus(UNINITIALIZED),
    mParent(NULL) {
  }
  
  pthread_linux::~pthread_linux() {
    if (mThreadStatus == RUNNING) {
      pthread_cancel(mThread);
    }
  }
  



  void pthread_linux::run() {
    mThreadStatus = INITIALIZED;
    int result;
    result = pthread_create( &mThread, 
			     NULL, 
			     static_cast<tThreadMain>(EnterMain),
			     static_cast<void*>(this));
    
    pthread_detach(mThread); // join不能

    if (result) {
      char buf[256];
      sprintf(buf, "pthread_create error #%d", result);
      throw new std::string(buf);
    }
  }
    



} // namespace


#endif
