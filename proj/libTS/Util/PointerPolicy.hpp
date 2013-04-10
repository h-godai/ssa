/* PointerPolicy.hpp   -*-tab-width:2-*-  */
/**
 * libTS PointerPolicy Template Class definition
 * Copyright (c) 2005-2010 TECHNICAL ARTS All Rights Reserved.
 *
 * @author h.godai godai@techarts.co.jp
 *
 * revision history
 * 2010-01-15 Created
 *
 */

#ifndef TS_UTIL_POINTER_POLICY_HPP
#define TS_UTIL_POINTER_POLICY_HPP

#include "../Common/Common.h"

#ifndef STDAFX_H
#if !TS_NO_BOOST
#include <boost/shared_ptr.hpp>
#endif
#include <vector>
#endif

#include "mpl.hpp"


namespace ts {

  template <typename Cls>
  struct ImmediatePointerPolicy {
    typedef Cls* ptr;
    static void Delete(ptr& p) {
      if (p != 0) delete p;
      p = 0;
    }
  };

#if !TS_NO_BOOST
  template <typename Cls>
  struct BoostSharedPtrPolicy {
    typedef typename boost::shared_ptr<Cls> ptr;
    static void Delete(ptr& p) {
      p = ptr();
    }
  };

  template <typename Cls>
  struct DefaultPointerPolicy : BoostSharedPtrPolicy<Cls> {
  };

  template <typename Ptr>
  bool isNull(boost::shared_ptr<Ptr> ptr) {
    return ptr.get() == 0;
  }

#else

  template <typename Cls>
  struct DefaultPointerPolicy : ImmediatePointerPolicy<Cls> {
  };

#endif


  template <typename Cls, template <class> class Policy = BoostSharedPtrPolicy >
  struct PointerPolicy : Policy<Cls> {
    typedef typename Policy<Cls>::ptr ptr;
    template <typename SubCls>
    struct SubClassPointerPolicy : Policy<SubCls> {
    };
  };

  template <typename Ptr>
  bool isNull(Ptr* ptr) {
    return ptr == 0;
  }


}



#endif

