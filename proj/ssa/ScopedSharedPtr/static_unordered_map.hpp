#pragma once

#include <unordered_map>
#include "static_allocator.hpp"
#include "static_string.hpp"

namespace ts {

  template <typename Key
			, typename T
			, size_t BufferBytes
			, typename Hash = std::hash<Key>
			, typename KeyEqual = std::equal_to<Key>
			>
  struct static_unordered_map_detail {
	using value_type = typename std::unordered_map<Key,T,Hash,KeyEqual>::value_type;
	using allocator = static_allocator<BufferBytes, value_type>;
	using map_type = std::unordered_map<Key,T,Hash,KeyEqual,typename allocator::alctype>;
	struct type : private allocator, map_type {
	  type() : map_type(0 // size_type __n = 10,
						, typename map_type::hasher()
						, typename map_type::key_equal()
						, allocator::get_allocator())
	  {}
	  type(std::initializer_list<value_type> init)
		: map_type(init
				   , 0 
				   , typename map_type::hasher()
				   , typename map_type::key_equal()
				   , allocator::get_allocator())
	  {}
	};
  };
  template <typename Key
			, typename T
			, size_t BufferBytes
			, typename Hash = std::hash<Key>
			, typename KeyEqual = std::equal_to<Key>
			>
  using static_unordered_map = typename static_unordered_map_detail<Key,T,BufferBytes,Hash,KeyEqual>::type;
  
}

#if 0
// これがやりたい
namespace std {
  
  template <size_t N>
  struct hash<typename ts::static_string_detail<char,N>::type> {
	size_t operator () (ts::static_string<N> const & key) const {
	  return key.length();
	}
  };
}
#endif
