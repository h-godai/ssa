
#ifndef TS_PTHREAD_WIN32_HPP
#define TS_PTHREAD_WIN32_HPP

#include "../ThreadBase.hpp"

#ifndef STDAFX_H
#ifndef _AFXDLL
#include	<windows.h>
#endif
#include	<process.h>
#endif

namespace ts {

  typedef CRITICAL_SECTION Mutex_t;
  typedef HANDLE Cond_t;

  class mutex_win32 : public IMutex {
  public:
    Mutex_t mMutex;
    mutex_win32() : m_initialized(false), m_finalized(false) {
    }

    ~mutex_win32() {
      finalize();
    }

    void initialize() {
      if (!m_initialized) {
        m_initialized = true;
	InitializeCriticalSection( &mMutex );
      }
    }

    void finalize() {
      if (!m_finalized) {
        m_finalized = true;
        if (m_initialized) {
          DeleteCriticalSection(&mMutex);
        }
      }
    }

    void lock() {
      if (!m_initialized) initialize();
      EnterCriticalSection(&mMutex);
    }

    void unlock() {
      if (!m_initialized) initialize();
      LeaveCriticalSection(&mMutex);
    }

  private:
    bool m_initialized;
    bool m_finalized;

  };

  class condition_win32 : public ICondition {
  public:
    Cond_t  m_cond;
    condition_win32() : m_initialized(false), m_finalized(false) {
    }

    ~condition_win32() {
      finalize();
    }

    void initialize() {
      if (!m_initialized) {
	m_cond = CreateEvent( NULL, TRUE, FALSE, NULL );
        m_initialized = true;
      }
    }

    void finalize() {
      if (m_initialized && !m_finalized) {
        CloseHandle(m_cond);
        m_finalized = true;
      }
    }

    status_t wait(int msec, IMutex* mtx) {
      if (!m_initialized) initialize();
      status_t result = NoError; 
      if (!m_initialized) initialize();
      if (msec == -1) msec = INFINITE;
      mtx->unlock();
      switch(WaitForSingleObject(m_cond, msec)) {
      case WAIT_FAILED:
        result = WaitFailed;
        break;
      case WAIT_ABANDONED:
        result = WaitFailed;
        break;
      case WAIT_OBJECT_0:
        result = WaitSignaled;
        break;
      case WAIT_TIMEOUT:
        result = WaitTimeout;
        break;
      }
      ResetEvent( m_cond );
      return result;
    }

    
    void signal() {
      if (!m_initialized) initialize();
      SetEvent( m_cond );
    }


  private:
    bool m_initialized;
    bool m_finalized;

  };

  class pthread_win32 : public ThreadBase {
  public:
    pthread_win32(tThreadMain main, void* ptr);
    virtual ~pthread_win32();
    virtual bool run();
    virtual void sleep(int msec); // sleep

  private:
    HANDLE m_thread;
  };

}
#endif