// -*-tab-width:4-*-

#pragma once

#include "libTS/Util/allocator.hpp"

namespace ts { namespace pool {

	template<typename BA> 
	struct allocator_base {
	  typedef BA BaseAllocator;
	  static BaseAllocator* defaultAllocator_ ;
	};

	template<class Typ, typename BA = ts::pool::simple_allocator<16> > 
	class legacy_allocator : public allocator_base<BA> {
	public:
	  typedef Typ value_type;
	  typedef legacy_allocator<Typ> other;
	  typedef value_type *pointer;
	  typedef const value_type *const_pointer;
	  typedef void *void_pointer;
	  typedef const void *const_void_pointer;
	  typedef value_type& reference;
	  typedef const value_type& const_reference;
	  typedef size_t size_type;
	  typedef ptrdiff_t difference_type;
	  typedef typename allocator_base<BA>::BaseAllocator BaseAllocator;

	  BaseAllocator* allocator_;
	  
	  using allocator_base<BA>::defaultAllocator_;

	  legacy_allocator() throw()
		: allocator_(defaultAllocator_)
	  {	// construct default allocator (do nothing)
		
	  }
	  
	  legacy_allocator(const legacy_allocator<Typ, BA>& a) throw() 
		: allocator_(a.allocator_)
		  {	// construct by copying (do nothing)
	  }
	  
	  template<class _Other>
	  legacy_allocator(const legacy_allocator<_Other, BA>& a) throw() 
		: allocator_(a.allocator_)
	  {	// construct from a related allocator (do nothing)
	  }


#if 0 && _HAS_CPP0X
	  typedef false_type propagate_on_container_copy_assignment;
	  typedef false_type propagate_on_container_move_assignment;
	  typedef false_type propagate_on_container_swap;
	  
	  legacy_allocator<Typ> select_on_container_copy_construction() const
	  {	// return this allocator
		return (*this);
	  }
 #endif /* _HAS_CPP0X */
	  
	  template<class _Other>
	  struct rebind {	// convert this type to allocator<_Other>
		typedef legacy_allocator<_Other, BA> other;
	  };
#if 0
	  pointer address(reference _Val) const 	{	// return address of mutable _Val
		return (addressof(_Val));
	  }
	  
	  const_pointer address(const_reference _Val) const 	{	// return address of nonmutable _Val
		return (addressof(_Val));
	  }
	  
#endif
	  
	  
	  template<class _Other>
	  legacy_allocator<Typ>& operator=(const legacy_allocator<_Other>& a)	{	// assign from a related allocator (do nothing)
		allocator_ = a.allocator_;
		return (*this);
	  }
	  
	  void deallocate(pointer ptr, size_type) {	// deallocate object at _Ptr, ignore size
		allocator_->free(ptr);
	  }
	  
	  pointer allocate(size_type count) {	// allocate array of _Count elements
		return static_cast<pointer>(allocator_->malloc(count * sizeof(Typ)));
	  }
	  
	  pointer allocate(size_type _Count, const void *) {	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
	  }

	  void construct(Typ *_Ptr) {	// default construct object at _Ptr
		::new ((void *)_Ptr) Typ();
	  }

	  void construct(Typ *_Ptr, const Typ& _Val) {	// construct object at _Ptr with value _Val
		::new ((void *)_Ptr) Typ(_Val);
	  }
#ifndef WIN32
	  template< class U, class... Args >
	  void construct( U* p, Args&&... args ) {
		::new((void *)p) U(std::forward<Args>(args)...);
	  }
#endif

#if 0
#define _ALLOC_MEMBER_CONSTRUCT( \
		TEMPLATE_LIST, PADDING_LIST, LIST, COMMA, CALL_OPT, X2, X3, X4) \
		template<class _Objty COMMA LIST(_CLASS_TYPE)> \
		void construct(_Objty *_Ptr COMMA LIST(_TYPE_REFREF_ARG)) \
		{	/* construct _Objty(Typpes...) at _Ptr */ \
			::new ((void *)_Ptr) _Objty(LIST(_FORWARD_ARG)); \
		}
		_VARIADIC_EXPAND_0X(_ALLOC_MEMBER_CONSTRUCT, , , , )
#undef _ALLOC_MEMBER_CONSTRUCT
#endif
		template<class _Uty>
		void destroy(_Uty *_Ptr) {	// destroy object at _Ptr
		  _Ptr->~_Uty();
		}
	  
	  size_t max_size() const throw() {	// estimate maximum array size
		return ((size_t)(-1) / sizeof (Typ));
	  }


	};
	

	template<class Typ, typename BA = ts::pool::simple_allocator<16> > 
	class my_allocator : public allocator_base<BA> {
	public:
	  typedef Typ value_type;
	  typedef typename allocator_base<BA>::BaseAllocator BaseAllocator;
	  BaseAllocator* allocator_;
	  
	  using allocator_base<BA>::defaultAllocator_;

	  my_allocator() throw()
	  : allocator_(defaultAllocator_)
	  {	// construct default allocator (do nothing)
	  }
	  
	  my_allocator(const my_allocator<Typ, BA>& a) throw() 
		: allocator_(a.allocator_)
	  {	// construct by copying (do nothing)
	  }
	  
	  template<class _Other>
	  my_allocator(const my_allocator<_Other, BA>& a) throw() 
		: allocator_(a.allocator_)
	  {	// construct from a related allocator (do nothing)
	  }

	  template<class _Other>
	  struct rebind {	// convert this type to allocator<_Other>
		typedef my_allocator<_Other, BA> other;
	  };


	  template < typename U > bool operator == ( my_allocator<U> const & o)  {
		return allocator_ == o.allocator_;
	  }
	  template < typename U > bool operator != ( my_allocator<U> const & o)  {
		return allocator_ != o.allocator_;
	  }
	  
	  
	  
	  template<class _Other>
	  my_allocator<Typ>& operator=(const my_allocator<_Other>& a)	{	// assign from a related allocator (do nothing)
		allocator_ = a.allocator_;
		return (*this);
	  }
	  
	  void deallocate(value_type* ptr, size_type) {	// deallocate object at _Ptr, ignore size
		allocator_->free(ptr);
	  }
	  
	  value_type* allocate(size_type count) {	// allocate array of _Count elements
		return static_cast<value_type*>(allocator_->malloc(count * sizeof(Typ)));
	  }
	  
	  value_type* allocate(size_type _Count, const void *) {	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
	  }

	  
	  size_t max_size() const throw() {	// estimate maximum array size
		return ((size_t)(-1) / sizeof (Typ));
	  }


	};

	
	template<typename BA>
	typename allocator_base<BA>::BaseAllocator* allocator_base<BA>::defaultAllocator_ = 0 ;
	

}} // ts::pool
