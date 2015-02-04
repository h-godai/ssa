#pragma once
#include <string>
#include "static_allocator.hpp"

namespace ts {
  
  template<typename T, size_t Elements>
  struct static_string_detail {
	// basic_stringは、文字列のサイズ + ポインタ3個分のメモリを使うようだ
	using allocator = static_allocator<sizeof(void*)*3+sizeof(T)*Elements, T>;
	using base = std::basic_string<T, std::char_traits<T>, typename allocator::alctype>;
	struct type : private allocator, base {
	  type() 
		: base(allocator::get_allocator()) 
	  {}
	  type(const T* text)
        : base(text, allocator::get_allocator()) 
	  {}
	  type(const base&& other)
	    : base(other, allocator::get_allocator()) 
	  {}
	  type(std::initializer_list<T> init)
		: base(init, allocator::get_allocator()) 
	  {}
	  struct hash {
		std::size_t operator () ( type const & key ) const {
		  return key.length();
		}
	  };
	};
  };
  
  template<size_t Elements>
  using static_string = typename static_string_detail<char,Elements>::type;
  template<size_t Elements>
  using static_wstring = typename static_string_detail<wchar_t,Elements>::type;
}

