template <class T>
struct custom_allocator {
  typedef T value_type;
  custom_allocator() noexcept {}
  template <class U> custom_allocator (const custom_allocator<U>&) noexcept {}
  T* allocate (std::size_t n) {
	std::cout << "malloc:" << typeid(T).name() << " " << n*sizeof(T) << "bytes" << endl;
	return reinterpret_cast<T*>(::malloc(n*sizeof(T)));
  }
  void deallocate (T* p, std::size_t n) {
	std::cout << "deallocate:" << n*sizeof(T) << endl;
	::free(p);
  }

  template<class Other>
  struct rebind {	// convert this type to allocator<_Other>
	typedef custom_allocator<Other> other;
  };

};
