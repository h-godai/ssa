// -*-tab-width:4-*-

#ifndef TS_UTIL_MEMORY_HPP
#define TS_UTIL_MEMORY_HPP

#include "singleton_policy.hpp"
#ifdef DEBUG
#include <typeinfo>
#endif
#include <stdio.h>
#include <map>
#include <string>

namespace ts {

  struct memory_debugger : singleton_policy<memory_debugger> {

    memory_debugger()
      : enable_(true)
    {}
    static void Clear() {
      Instance().count_.clear();
    }
    static void Enable() {
      Instance().enable_ = true;
    }
    static void Disable() {
      Instance().enable_ = false;
    }

    ~memory_debugger() {
      std::map<std::string, int>::iterator i;
      fprintf(stderr, "*** libTS Memory Debugger ***\n");
      int n = 0;
      for (i = count_.begin(); i != count_.end(); ++i) {
	if (i->second != 0) {
	  printf("Memory Leak! class='%s' count='%d'\n", i->first.c_str(), i->second);
	  n += i->second;
	}
      }
      if (n == 0) 
	fprintf(stderr, "*** no memory leaked. (^^) ***\n");
      else if (n < 32)
	fprintf(stderr, "*** %d count memory leaked! (X_x) ***\n", n);
      else
	fprintf(stderr, "*** %d count memory leaked! (@o@) ***\n", n);
    }

    template <typename T> static T* NewCnt(T* p) { return Instance().nw(p); }
    template <typename T> static T* DelCnt(T* p) { return Instance().dl(p); }
    
    template <typename T>
    T* nw(T* p) {
#ifdef DEBUG
      if (enable_) {
		const std::type_info & id = typeid(*p);
		++count_[id.name()];
      }
#endif
      return p;
    }
    template <typename T>
    T* dl(T* p) {
#ifdef DEBUG
      if (enable_) {
		if (p != 0) {
		  const std::type_info & id = typeid(*p);
		  --count_[id.name()];
		}
      }
#endif
      return p;
    }
    std::map<std::string, int> count_;
    bool enable_;
  };
  
#ifdef DEBUG
#define TS_NEWCOUNT(x) memory_debugger::NewCnt(x)
#define TS_DELCOUNT(x) memory_debugger::DelCnt(x)
#else
#define TS_NEWCOUNT(x) x
#define TS_DELCOUNT(x) x
#endif


  template <typename T>
  inline T* ts_new() {
    return TS_NEWCOUNT(new T());
  }
  template <typename T, typename A1>
  inline T* ts_new(A1& a1) {
    return TS_NEWCOUNT(new T(a1));
  }
  template <typename T, typename A1, typename A2>
  inline T* ts_new(A1& a1, A2& a2) {
    return TS_NEWCOUNT(new T(a1, a2));
  }
  template <typename T>
  inline void ts_delete(T* p) {
    delete TS_DELCOUNT(p);
  }









}

#endif
