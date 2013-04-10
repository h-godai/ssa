/* pthread_win32.cpp  -*-C++-*-


  win32版では、デバッグのためにMFCライブラリを使用する

*/
#ifdef	WIN32
#define WINVER 0x0600
#if defined(_AFXDLL)
#include <afxwin.h>
#include <afxext.h>         // MFC の拡張部分
#if  defined(_DEBUG)
# define new DEBUG_NEW
#endif
#else
#include <windows.h>
#endif

#include <process.h>
#include <stdio.h>

#include "pthread_win32.hpp"




namespace ts {


  pthread_win32::pthread_win32(tThreadMain main, void* ptr) 
    : ThreadBase(main, ptr), m_thread(0) {
  }

  pthread_win32::~pthread_win32() {
  }


  bool pthread_win32::run() {
    m_threadStatus = INITIALIZED;
    if ((m_thread = (HANDLE)_beginthread(reinterpret_cast<void(*)(void*)>(ThreadBase::enterMain), 0, (void*)this )) == (HANDLE)-1L ) {
      m_thread = 0;
      return false;
    }
    return true;
  }

  void pthread_win32::sleep(int msec) {
    mutex_win32 mtx;
    condition_win32 cond;
    cond.wait(msec, &mtx);
  }

   

  // pthread factory
  IThread* IThread::createThread(tThreadMain main, void* ptr) {
    return new pthread_win32(main, ptr);
  }

  void IThread::deleteThread(IThread* thread) {
    delete thread;
  }

  // mutex factory
  IMutex* IMutex::createMutex() {
    return new mutex_win32;
  }

  void IMutex::deleteMutex(IMutex* mtx) {
    delete mtx;
  }

  // condition factory
  ICondition* ICondition::createCondition() {
    return new condition_win32;
  }

  void ICondition::deleteCondition(ICondition* mtx) {
    delete mtx;
  }


} // namespace


#endif


