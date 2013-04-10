/* threadproc.cpp  -*-C++-*-

	solaris??????????

              maeda
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#include <thread.h>
#include "thread_solaris.hpp"


namespace tsl {
  
  typedef mutex_t Mutex_t;
  typedef cond_t Cond_t;
  
  class MutexSolaris : public IMutex {
  public:
    Mutex_t mMutex;
    MutexSolaris() {
      initialize();
    }

    ~MutexSolaris() {
      finalize();
    }

    void initialize() {
      mutex_init(&mMutex, USYNC_THREAD, NULL); // 
    }

    void finalize() {
      mutex_destroy(&mMutex);
    }

    void lock() {
      mutex_lock(&mMutex);
    }

    void unlock() {
      mutex_unlock(&mMutex);
    }

    Mutex_t* getMutex() {
      return &mMutex;
    }
    
  };

  class ConditionSolaris : public ICondition {
  public:
    Cond_t  mCond;
    ConditionSolaris() {
      initialize();
    }

    ~ConditionSolaris() {
      finalize();
    }

    void initialize() {
      cond_init(&mCond, USYNC_THREAD, NULL);
      //	mCond	= PTHREAD_COND_INITIALIZER;
    }

    void finalize() {
      cond_destroy(&mCond);
    }

    status_t wait(int msec, IMutex* mutex ) {
      int result;
      MutexSolaris* mtx = dynamic_cast<MutexSolaris*>(mutex);
      if (mtx == 0) return WaitFailed;

      mutex->lock();

      if (msec == -1) {
        result = ::cond_wait(&mCond, mtx->getMutex() );
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
          // gettimeofday?Ž¸??????b??
          t.tv_sec = time(NULL) + (msec / 1000);
          t.tv_nsec = (msec % 1000) * 1000000;
        }
        result = cond_timedwait(&mCond, mtx->getMutex(), &t);
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
      ::cond_signal(&mCond);
    }

  };

  thread_solaris::thread_solaris(tThreadMain main, void* ptr)
       : ThreadBase(main, ptr),
         m_thread(0) {

         }

  thread_solaris::~thread_solaris() {
    if (m_threadStatus == RUNNING) {
      //thr_exit(m_thread);
      //thread_cancel(m_thread);
    }
  }



  bool thread_solaris::run() {
    m_threadStatus = INITIALIZED;
    int result;
    result = thr_create( NULL, 0,
                         ThreadBase::enterMain,
                         //static_cast<tThreadMain>(ThreadBase::enterMain),
                         static_cast<void*>(this),
                         0,
                         &m_thread
                         );

    //thread_detach(m_thread); /* join?? */

    if (result) {
      char buf[256];
      sprintf(buf, "thread_create error #%d", result);
      throw new std::string(buf);
    }
    return true;
  }

  void thread_solaris::sleep(int msec) {
    struct timespec t, rem;
    t.tv_nsec = (msec % 1000) * 1000000;
    t.tv_sec = msec / 1000;
    while (nanosleep(&t, &rem) < 0) {
      if (errno != EINTR) break;
      t = rem;
    }
  }

  IThread* IThread::createThread(tThreadMain main, void* ptr) {
    return new thread_solaris(main, ptr);
  }

  void IThread::deleteThread(IThread* thread) {
    delete thread;
  }


  // mutex factory
  IMutex* IMutex::createMutex() {
    return new MutexSolaris;
  }

  void IMutex::deleteMutex(IMutex* mtx) {
    delete mtx;
  }

  // condition factory
  ICondition* ICondition::createCondition() {
    return new ConditionSolaris;
  }

  void ICondition::deleteCondition(ICondition* mtx) {
    delete mtx;
  }

} // namespace



