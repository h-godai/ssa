#ifndef TS_SHARED_MEMORY_CTRL
#define TS_SHARED_MEMORY_CTRL

#include <stdexcept>
#include "../../Common/PlatformType.hpp"


namespace ts {

  struct SharedMemoryException : std::exception {
    SharedMemoryException(const char* msg) throw() : msg_(msg) {}
    virtual ~SharedMemoryException() throw() {}
    virtual const char* what() const throw() { return msg_.c_str(); }
    std::string msg_;
  };

namespace shm {

  template <ts::PlatformType PLATFORM>
  struct SharedMemoryCtrl {
  };

}} // namespace


#ifdef TS_PLATFORM_LINUX
#include "SharedMemoryCtrl_linux.hpp"
#endif






#endif

