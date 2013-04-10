/* -*-C++-*-

  pthread proc class


*/


#ifndef TS_IThread_H
#define TS_IThread_H


namespace ts {
  
  class IMutex {
  public:
    IMutex() {};
    virtual ~IMutex() {};
    virtual void initialize() = 0;
    virtual void finalize() = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    static IMutex* createMutex(); // factory
    static void deleteMutex(IMutex*); // factory

  };

  // mutexのlock/unlockを自動で行うためのクラス
  struct MutexLock {
    IMutex* mutex;
    MutexLock(IMutex* mutex_) : mutex(mutex_) {
      mutex->lock();
    }
    ~MutexLock() {
      mutex->unlock();
    }
  };

  class ICondition {
  public:
    enum  status_t {
      NoError,
      WaitFailed,
      WaitSignaled,
      WaitTimeout,
    } ;

    ICondition() {}
    virtual ~ICondition() {}
    virtual void initialize() = 0;
    virtual void finalize() = 0;
    virtual status_t wait(int timeout, IMutex*) = 0;
    virtual void signal() = 0;
    static ICondition* createCondition(); // factory
    static void deleteCondition(ICondition*); // factory
  };

  class IThread {
  public:
    typedef void* (*tThreadMain)(IThread*, void*);
    typedef enum {
      UNINITIALIZED, // 0
      INITIALIZED,   // 1
      RUNNING,       // 2
      WAITING,       // 3
      END,           // 4
      BOTH           // 5
    } tThreadStatus;
    
  protected:
    IThread() {}
  public:
    virtual ~IThread() {}

    static IThread* createThread(tThreadMain main, void*);
    static void deleteThread(IThread*);
    
    virtual bool run() = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual ICondition::status_t wait(int msec=-1) = 0;  // タイムアウトするかシグナルが来るまで待つ
    virtual void sleep(int msec) = 0; // sleep
    virtual void signal() = 0;
    virtual tThreadStatus getThreadStatus() const = 0;
    virtual const IMutex* getMutex() const = 0;
    virtual void setMainRoutine(tThreadMain main) = 0;
    virtual bool waitForStatusIs(tThreadStatus target, int timeout) = 0;
    virtual bool join() = 0;
  };


} // namespace


#endif
