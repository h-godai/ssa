#pragma once

#include <map>
#include <tuple>
#include <unordered_map>
#include "static_allocator.hpp"

namespace ts {

  template <typename Key, typename T, size_t MaxElements,  class Cmp = std::less<Key> >
  struct static_map_detail {
	using value_type = typename std::map<Key,T>::value_type;
	using sizetest = std::tuple<Key,T,static_allocator<0>::alctype,void*,void*>;
	using allocator = static_allocator<MaxElements * sizeof(sizetest), value_type>;
	using map_type = std::map<Key, T, Cmp, typename allocator::alctype>;
	struct type : private allocator, map_type {
	  type(const Cmp& comp = Cmp())
		: map_type(comp, allocator::get_allocator()) {}
	  type(std::initializer_list<value_type> init
		   , const Cmp& comp = Cmp())
		: map_type(init, comp, allocator::get_allocator()) {}
	};
  };
  
  template <typename Key, typename T, size_t BufSz, class Cmp = std::less<Key> >
  using static_map = typename static_map_detail<Key,T,BufSz,Cmp>::type;

  
}
