// -*-tab-width:4-*-
#ifndef TS_UTIL_SINGLETON_POLICY_HPP
#define TS_UTIL_SINGLETON_POLICY_HPP

namespace ts {

  // singleton policy
  template <class Cls> 
  struct singleton_policy {
    typedef singleton_policy Type;
    static Cls& Instance() {
      static Cls instance;
      return instance;
    }
    static Cls* Instance_ptr() {
      return &Instance();
    }
  };
}
#endif
