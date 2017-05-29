/* threadproc.cpp  -*-C++-*-

	linux用スレッドライブラリ

*/
#ifdef __LINUX__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#include <pthread.h>
#include "pthread_linux.hpp"


namespace ts {

  //  pthread_cond_t  CondInitializer = PTHREAD_COND_INITIALIZER;
  //  pthread_mutex_t MutexInitializer = PTHREAD_MUTEX_INITIALIZER;

  typedef pthread_mutex_t Mutex_t;
  typedef pthread_cond_t Cond_t;

  class MutexLinux : public IMutex {
  public:
    Mutex_t mMutex;
    MutexLinux() {
	  initialize();
    }

    ~MutexLinux() {
	  finalize();
    }

    void initialize() {
	  pthread_mutex_init(&mMutex, NULL);
	  //	mMutex	= PTHREAD_MUTEX_INITIALIZER;
    }

    void finalize() {
	  pthread_mutex_destroy(&mMutex);
    }

    void lock() {
	  pthread_mutex_lock(&mMutex);
    }

    void unlock() {
	  pthread_mutex_unlock(&mMutex);
    }

    Mutex_t* getMutex() {
      return &mMutex;
    }

  };

  class ConditionLinux : public ICondition {
  public:
    Cond_t  mCond;
    ConditionLinux() {
	  initialize();
    }

    ~ConditionLinux() {
	  finalize();
    }

    void initialize() {
      pthread_cond_init(&mCond, NULL);
      //	mCond	= PTHREAD_COND_INITIALIZER;
    }

    void finalize() {
      pthread_cond_destroy(&mCond);
    }


    status_t wait(int msec, IMutex* mutex) {
      int result;
      //printf("Enter cond_wait\n");
      MutexLinux* mtx = dynamic_cast<MutexLinux*>(mutex);
      if (mtx == 0) return WaitFailed;
      mutex->lock();
      if (msec == -1) {
        result = pthread_cond_wait(&mCond, mtx->getMutex());
      }
      else {
        struct timeval now;
        struct timespec t;
        if (!gettimeofday(&now, NULL)) {
	  now.tv_usec += (msec % 1000) * 1000;
	  if(now.tv_usec > 1000000) {
	    now.tv_sec += 1;
	    now.tv_usec -= 1000000;
	  }
	  t.tv_sec = now.tv_sec + (msec / 1000);
	  t.tv_nsec = now.tv_usec * 1000;
        }
        else {
	  // gettimeofdayに失敗した場合は秒単位
	  t.tv_sec = time(NULL) + (msec / 1000);
	  t.tv_nsec = (msec % 1000) * 1000000;
        }
        result = pthread_cond_timedwait(&mCond, mtx->getMutex(), &t); 
      }
      mutex->unlock();
      switch(result) {
      case ETIMEDOUT: 
	//printf("ETIMEDOUT\n");
	return WaitTimeout;
      case 0:
      case EINTR:
	//printf("EINTR\n");
	return WaitSignaled;
      case EBUSY:
	//printf("EBUSY\n");
	return WaitFailed;
      default:
	//printf("pthread_cond_wait failed\n");
	return WaitFailed;
      }

    }
    
    void signal() {
      pthread_cond_signal(&mCond);
    }

  };

  pthread_linux::pthread_linux(tThreadMain main, void* ptr)
    : ThreadBase(main, ptr),
      m_thread(0) {

  }
  
  pthread_linux::~pthread_linux() {
    if (m_threadStatus == RUNNING) {
      pthread_cancel(m_thread);
    }
  }
  


  bool pthread_linux::run() {
    m_threadStatus = INITIALIZED;
    int result;
    result = pthread_create( &m_thread, 
			     NULL, 
			     ThreadBase::enterMain,
			     static_cast<void*>(this));
    
    pthread_detach(m_thread); /* join不能 */
    
    if (result) {
      char buf[256];
      sprintf(buf, "pthread_create error #%d", result);
      throw new std::string(buf);
    }
    return true;
  }

  void pthread_linux::sleep(int msec) {
    struct timespec t, rem;
    t.tv_nsec = (msec % 1000) * 1000000;
    t.tv_sec = msec / 1000;
    while (nanosleep(&t, &rem) < 0) {
      if (errno != EINTR) break;
      t = rem;
    }
  }


  IThread* IThread::createThread(tThreadMain main, void* ptr) {
    return new pthread_linux(main, ptr);
  }

  void IThread::deleteThread(IThread* thread) {
    delete thread;
  }

  IMutex* IMutex::createMutex() {
    return new MutexLinux;
  }
  void IMutex::deleteMutex(IMutex* mutex) {
    delete mutex;
  }

    // condition factory
  ICondition* ICondition::createCondition() {
    return new ConditionLinux;
  }

  void ICondition::deleteCondition(ICondition* mtx) {
    delete mtx;
  }


} // namespace


#endif
