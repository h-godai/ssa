#pragma once
#include <vector>
#include "static_allocator.hpp"

namespace ts {
  
  template<typename T, size_t Elements>
  struct static_vector_detail {
	using allocator = static_allocator<sizeof(T)*Elements, T>;
	using base = std::vector<T, typename allocator::alctype>;
	struct type : private allocator, base {
	  type() : base(allocator::get_allocator()) {
		base::reserve(Elements);
	  }
	  type(const std::vector<T>&& other)
		: base(other, allocator::get_allocator()) {
	  }
	  type(std::initializer_list<T> init)
		: base(init, allocator::get_allocator()) {
	  }
	};
  };
  
  template<typename T, size_t Elements>
  using static_vector = typename static_vector_detail<T,Elements>::type;

}
