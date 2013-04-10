

#ifndef TS_PTHREAD_LINUX_HPP
#define TS_PTHREAD_LINUX_HPP

#include "../pthread.hpp"

#ifdef __LINUX__

namespace tsl {

  class pthread_linux : public pthread {
  public:
    pthread_linux();
    virtual ~pthread_linux();
    virtual bool run();

  private:
    pthread_t m_thread;
  };

}

#endif
#endif
