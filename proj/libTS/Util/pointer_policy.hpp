// -*-tab-width:4-*-
#ifndef TS_UTIL_POINTER_POLICY_HPP
#define TS_UTIL_POINTER_POLICY_HPP

#include "libTS/Common/Common.h"
#if !TS_NO_BOOST
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
//#include <boost/interprocess/detail/atomic.hpp>
//#include <boost/atomic.hpp>
#include <boost/smart_ptr/detail/spinlock.hpp>
#define TS_HAS_INTRUSIVE_PTR
#endif

#include "singleton_policy.hpp"
#include "memory.hpp"

namespace ts {

#if !TS_NO_BOOST
  using boost::shared_ptr;
  using boost::intrusive_ptr;
#endif

  // ただのポインタの実装
  template <typename T>
  struct immidiate_pointer_policy_impl {
	struct ptr {
	  T* ptr_;
	  ptr(T* p) : ptr_(p) {}
	  T& operator * () { return *ptr_; }
	  T& operator -> () { return *ptr_; }
	  const T& operator * () const { return *ptr_; }
	  const T& operator -> () const { return *ptr_; }
	  T* get() { return ptr_; }
	  const T* get() const { return ptr_; }
	};
    static ptr Create() { return ts_new<T>(); }
    template <typename A1>
	static ptr Create(A1& a1) { return ts_new<T>(a1); }
    template <typename A1, typename A2> 
	static ptr Create(A1& a1, A2& a2) { return ts_new<T>(a1,a2); }
    template <typename A1, typename A2, typename A3>
	static ptr Create(A1& a1, A2& a2, A3& a3) { return ts_new<T>(a1,a2,a3); }
    static void Destroy(ptr p) { ts_delete<T>(p); }
    template <typename P>
    static ptr DynamicCast(P p) { return dynamic_cast<T*>(p); }
	template <typename TT>
    static ptr toPtr(TT* p) { return p; }
	
  };

   
  // boostタイプのshared_ptrポリシー
  template <typename T>
  struct shared_ptr_policy_impl {
    typedef shared_ptr<T> ptr;
    static void Destroy(ptr) {  }
    template <typename P>
    static ptr DynamicCast(P p) { return boost::dynamic_pointer_cast<T>(p); }
	template <typename TT>
    static ptr toPtr(T* p) { return ptr(p); }
  };

#ifdef TS_HAS_INTRUSIVE_PTR
  template <typename T>
  class intrusive_ptr_policy_impl {
  private:
	//mutable boost::atomic<int> refcount_;
	mutable int refcount_;
	mutable boost::detail::spinlock lock_;
  public:
	typedef boost::intrusive_ptr<T> ptr;
	template <typename TT>
	struct get_ptr {
	  typedef boost::intrusive_ptr<TT> type;
	};

	intrusive_ptr_policy_impl() : refcount_(0) {}

    static void Destroy(ptr) {  }
    template <typename P>
    static ptr DynamicCast(P p) { return boost::dynamic_pointer_cast<T>(p); }
	template <typename TT>
    static ptr toPtr(T* p) { return ptr(p); }

	friend void intrusive_ptr_add_ref(T* x)
	{
	  //x->refcount_.fetch_add(1, boost::memory_order_relaxed);
	  //x->lock_.lock();
	  ++x->refcount_;
	  //x->lock_.unlock();
	}
	friend void intrusive_ptr_release(T* x)
	{
	  //if (x->refcount_.fetch_sub(1, boost::memory_order_release)==1) {
	  //boost::atomic_thread_fence(boost::memory_order_acquire);
	  //delete x;
	  //}
	  //x->lock_.lock();
	  if (--x->refcount_ == 0) {
		//x->lock_.unlock();
		delete x;
		return;
	  }
	  //x->lock_.unlock();
	}
  };
#endif


  template <typename Cls, template <typename C> class PointerPolicy >
  struct pointer_policy_impl : PointerPolicy<Cls> {
    typedef typename PointerPolicy<Cls>::ptr ptr;

    // 自分自身のCreate
#if 0
    static ptr Create() { return PointerPolicy<Cls>::Create(); }
    template <typename A1>
    static ptr Create(A1& a1) { return PointerPolicy<Cls>::Create(a1); }
    template <typename A1, typename A2>
    static ptr Create(A1& a1, A2& a2) { return PointerPolicy<Cls>::Create(a1,a2); }


    // 自分以外のCreate
    template <typename C>
    static typename C::ptr Create() { return PointerPolicy<C>::Create();}
    template <typename C, typename A1>
    static typename C::ptr Create(A1& a1) { return PointerPolicy<C>::Create(a1); }
    template <typename C, typename A1, typename A2>
    static typename C::ptr Create(A1& a1, A2& a2) { return PointerPolicy<C>::Create(a1,a2); }
    // 自分の破棄
    static void Destroy(ptr p) { PointerPolicy<Cls>::Destroy(p); }
#endif

    // 自分以外の破棄
    template <typename C>
    static void Destroy(typename C::ptr p) { PointerPolicy<C>::Destroy(p); }


#if 0
    // 自分へDynamicCast
    template <typename P>
    static ptr DynamicCast(P p) { return PointerPolicy<Cls>::DynamicCast(p); }
#endif

    // 自分以外のDynamicCast
    template <typename T, typename P>
    static typename PointerPolicy<T>::ptr DynamicCast(P p) { return PointerPolicy<T>::DynamicCast(p); }
    
  };

  // ただのポインタ
  template <typename Cls>
  struct immidate_pointer_policy : pointer_policy_impl<Cls, immidiate_pointer_policy_impl> {
    template <typename T> struct PointerPolicy : immidiate_pointer_policy_impl<T> {};
  };
  
  // boostタイプのshared_ptrポリシー
  template <typename Cls>
  struct shared_ptr_policy  : pointer_policy_impl<Cls, shared_ptr_policy_impl> {
    template <typename T> struct PointerPolicy : shared_ptr_policy_impl<T> {};
  };

  // intrusive_ptrrポリシー
  template <typename Cls>
  struct intrusive_ptr_policy  : pointer_policy_impl<Cls, intrusive_ptr_policy_impl> {
    template <typename T> struct PointerPolicy : intrusive_ptr_policy_impl<T> {};
  };



#if TS_NO_BOOST
  // boostを使わない
  template <typename Cls>
  struct default_pointer_policy : immidate_pointer_policy<Cls> {
  };

#else
  using boost::shared_ptr;
  template <typename Cls>
  struct default_pointer_policy : shared_ptr_policy<Cls> {
  };
#endif

}









#endif

