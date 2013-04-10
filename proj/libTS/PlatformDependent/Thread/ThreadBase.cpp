/* pthread.cpp  -*-C++-*-



*/
#include <stdio.h>
#include "ThreadBase.hpp"

#if defined(WIN32) && defined(_DEBUG) && defined(_AFXDLL)
#define WINVER 0x0502
#include <afx.h>
#define new DEBUG_NEW
#endif
#ifdef WIN32
#pragma warning (disable : 4996)
#endif

namespace ts {



  int ThreadBase::m_maxThreads = 0;
  int ThreadBase::m_threads = 0;

  ThreadBase::ThreadBase(tThreadMain main, void* ptr) 
    : m_threadStatus(UNINITIALIZED),
      m_mainRoutine(main),
      m_userPtr(ptr) 
  {
    m_mutex = IMutex::createMutex();
    m_condition = ICondition::createCondition();
    m_eventCondition = ICondition::createCondition();
    if (++m_threads > m_maxThreads) m_maxThreads = m_threads;

  }
  
  ThreadBase::~ThreadBase() {
    IMutex::deleteMutex(m_mutex);
    ICondition::deleteCondition(m_condition);
    ICondition::deleteCondition(m_eventCondition);
    --m_threads;
  }

  void* ThreadBase::enterMain(void* ptr) {
    ThreadBase* my = static_cast<ThreadBase*>(ptr);
    my->m_threadStatus = RUNNING;
    my->m_eventCondition->signal();
    if (my->m_mainRoutine != 0) {
      my->m_mainRoutine(my, my->m_userPtr);
    }
    my->m_threadStatus = END;
    my->m_eventCondition->signal();
    return 0;
  }

  // ステータスがtaretになるかtimeoutするまでまで待つ。
  // targetがBOTHの時は、ステータスセットのイベントが来たら戻る
  bool ThreadBase::waitForStatusIs(tThreadStatus target, int timeout) {
    //m_eventCondition->wait(1, m_mutex); // reset signal
    while (m_threadStatus != target) {
      switch(m_eventCondition->wait(timeout, m_mutex)) {
      case ICondition::WaitTimeout: return false;
      case ICondition::WaitSignaled:
        if (target == BOTH || m_threadStatus == target) return true;
      default:
	break;
      }
    }
    return true;
  }

  bool ThreadBase::join() {
    if (m_threadStatus == UNINITIALIZED) return false;
    while (m_threadStatus != END) {
      waitForStatusIs(END, 100);
    }
    return true;
  }


} // namespace

