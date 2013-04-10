
#ifndef TS_UTIL_RAND_HPP
#define TS_UTIL_RAND_HPP


#include <sys/time.h>
#include <stdint.h>
#include <boost/random.hpp>
#include "policy.hpp"
#include <limits>

namespace ts {

  template <typename T>
  struct Random : singleton_policy<Random<T> > {
    // 乱数ジェネレータ
    boost::mt19937 gen_;
    boost::uniform_int<T> dst_;
    boost::variate_generator< boost::mt19937, boost::uniform_int<T> > rand_;
    
    Random()
      : gen_(Seed())
      , dst_(std::numeric_limits<T>::min(), std::numeric_limits<T>::max())
      , rand_(gen_, dst_)
    {}

    static T Seed() {
      struct timeval tv;
      gettimeofday(&tv, 0);
      return static_cast<T>(tv.tv_usec);
    }

    static T Get() { return Random::Instance().rand_(); }

  };
  
}
#endif
  

