/* thread_proc.hpp

  スレッドによって実行されるプロシージャを定義する親クラス

  */



#ifndef TS_UTIL_THREAD_PROC_HPP
#define TS_UTIL_THREAD_PROC_HPP

#include <exception>
#include "libTS/PlatformDependent/Thread/IThread.hpp"

namespace ts {

  struct thread_proc_exception : std::exception {
    thread_proc_exception(const char * const & msg) : std::exception(msg) {}
  };

  template<typename CLS>
  class thread_proc {
  protected:
    thread_proc() : thread_(0) {}
    ~thread_proc() {
      if (thread_ != 0) {
        IThread::deleteThread(thread_);
      }
    }
  public:

    static void* threadMain(IThread* , void* ins) {
      CLS* instance = reinterpret_cast<CLS*>(ins);
      instance->main();
      return 0;
    };


    void run() {
      if (thread_ != 0) {
        IThread::deleteThread(thread_);
      }
      thread_ = IThread::createThread(threadMain, reinterpret_cast<void*>(this));
      thread_->run();
    }

    void signal() {
      if (thread_ == 0) throw thread_proc_exception("thread_ is null");
      thread_->signal();
    }
    void lock() {
      if (thread_ == 0) throw thread_proc_exception("thread_ is null");
      thread_->lock();
    }
    void unlock() {
      if (thread_ == 0) throw thread_proc_exception("thread_ is null");
      thread_->unlock();
    }
    void join() {
      if (thread_ == 0) throw thread_proc_exception("thread_ is null");
      thread_->join();
    }

  private:
    IThread* thread_;
  };


} // namespace ts
#endif

