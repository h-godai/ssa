/* -*-C++-*-

  pthread proc class


*/


#ifndef TS_PTHREAD_H
#define TS_PTHREAD_H

#include "IThread.hpp"

#ifdef __cplusplus

namespace ts {
  

  class ThreadBase : public IThread {
  public:
  protected:
    ThreadBase(tThreadMain, void*);
  public:
    virtual ~ThreadBase();

    static void* enterMain(void* ptr);
    
    virtual bool run() = 0;

    void lock() { m_mutex->lock(); }
    
    void unlock() { m_mutex->unlock(); }

    ICondition::status_t wait(int msec=-1) { return m_condition->wait(msec, m_mutex); }

    void signal() { m_condition->signal(); }

    tThreadStatus getThreadStatus() const { return m_threadStatus; }
    const IMutex* getMutex() const { return m_mutex; }
    const ICondition* getCondition() const { return m_condition; }

    void setMainRoutine(tThreadMain main) { m_mainRoutine = main; }

    bool waitForStatusIs(tThreadStatus target, int timeout);

    virtual bool join();


    static int getMaxThreads() { return m_maxThreads; } //< 同時に作成した最大スレッド数を返す
    static int getThreds() { return m_threads; } //< 現在のスレッド数を返す

  protected:
    volatile tThreadStatus m_threadStatus;
    IMutex* m_mutex;
    ICondition* m_condition;
    ICondition* m_eventCondition;
    tThreadMain m_mainRoutine;
    void* m_userPtr;

  private:
    static int m_maxThreads;
    static int m_threads;
  };


} // namespace



#endif // ifdef __cplusplus



#endif
