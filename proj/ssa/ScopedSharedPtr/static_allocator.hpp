// static_allocator.hp
#pragma once
#include <iostream>
#include <cstdint>
#include <exception>
#include <typeinfo>

namespace ts {
  
  template <size_t BufferByteSize, typename ElementType=void*>
  struct static_allocator {
	template <typename T>
	struct allocator_detail {
	  uint8_t** buffer_ = nullptr;
	  size_t* size_left_ = nullptr;
	  using value_type = T;
	  using element_type = T;
	  // これらの定義は、gcc4.8.1で必要になる。clang3.4では不要
	  using pointer = T*;
	  using const_pointer = const T*;
	  using reference = T&;
	  using const_reference = const T&;
	  using size_type = size_t;
	  using difference_type = std::ptrdiff_t;

	  // 確保するメモリーのポインタをサイズを受け取る。
	  // statefulなallocatorで可能になりました。
	  allocator_detail(uint8_t** buffer, size_t* buffersize) noexcept
	  : buffer_(buffer), size_left_(buffersize)
	  {}

	  // rebindで異なるクラス向けのallocatorを生成するときに呼ばれる
	  // コンストラクタ。インスタンスを引き継ぐための重要な処理です。
	  template <class U> allocator_detail(const allocator_detail<U>& other) noexcept
		: buffer_(other.buffer_), size_left_(other.size_left_)
	  {}

	  // gcc4.8.1のbasic_stringでこれがないとエラーになる
	  allocator_detail() {}

	  // メモリをアロケート
	  pointer allocate(std::size_t n) const {
		size_t required = n * sizeof(value_type);
		std::cout << "malloc:" << typeid(value_type).name() << " " << required << "bytes " << int32_t(*size_left_ - (signed)required) << "bytes left" << std::endl;
		return allocate_buffer(required);
	  }
	  // メモリを解放
	  void deallocate(pointer p, std::size_t n) const {
		size_t deallocsize = n * sizeof(value_type);
		// std::cout << "deallocate:" << typeid(value_type).name() << " " << deallocsize << "bytes" << std::endl;
		// 解放するポインターが最後にアロケートしたバッファだった場合、再利用可能にする
		if (reinterpret_cast<uint8_t*>(p) == *buffer_ - deallocsize) {
		  allocate_buffer(-deallocsize);
		}
	  }
	  // allocatorを異なる型用に再生成するためのクラス
	  template<class Other>
	  struct rebind : allocator_detail<Other> {
		typedef allocator_detail<Other> other;
	  };


	  // clangではdestroyとconstructは記述しなくても実行できるが、gcc4.8.1では無いとダメ
	  static void destroy(pointer& p) { p->~T(); }

	  // clangでは、value_type以外の型で呼ばれるためテンプレート化
	  template<typename U, typename ...Args>
	  static void construct(U* p, Args&&... args) { 
		::new (static_cast<void*>(p)) U(args...);
	  }
	  bool operator == (const allocator_detail<T>&) const {
		return true;
	  }
	  bool operator != (const allocator_detail<T>&) const {
		return false;
	  }
	  
	private:
	  // メモリ確保の処理 失敗したらout_of_rangeをthrow
	  pointer allocate_buffer(int32_t size) const {
		if (size > 0 && *size_left_ < size) {
		  std::cerr << "out of memory capacity:" << *size_left_ << " required:" << size << std::endl;
		  throw std::out_of_range("static_allocator out of capacity");
		}
		void* ptr = *buffer_;
		*buffer_ += size;
		*size_left_ -= size;
		return static_cast<pointer>(ptr);
	  }
	};

	// static_allocatorの実装はここから

	// ElementTypeのallocatorの型をtypeとして定義
	template <typename T>
	using allocator = allocator_detail<T>;

	using alctype = allocator<ElementType>;

	// allocator<ElementType>のインスタンスを得る
	allocator<ElementType> operator () () {
	  return get<ElementType>();
	}
	allocator<ElementType> get_allocator() {
	  return get<ElementType>();
	}
	

	// allocator<T>のインスタンスを得る
	template <typename T>
	allocator<T> get() {
	  return allocator<T>(&buffer_ptr_, &size_left_);
	}
  private:
	// これらのインスタンスは、スコープ内ならスタック領域に、
	// スコープ外ならグローバル領域に生成される。
	// 必要なサイズは、バッファサイズ+ sizeof(*) + sizeof(size_t)
	uint8_t  buffer_[BufferByteSize];      // allocateされるバッファ
	uint8_t* buffer_ptr_ = buffer_;        // 現在のバッファ位置
	size_t   size_left_  = BufferByteSize; // 確保可能な残りのサイズ
  };

  


}

// アロケーターを比較するための演算子の定義が必要らしい
template <size_t B, typename V, typename T, typename U>
bool operator == (const typename ts::static_allocator<B,V>::template allocator<T>&
			    , const typename ts::static_allocator<B,V>::template allocator<U>&)
{ return true; }
template <size_t B, typename V, typename T, typename U>
bool operator != (const typename ts::static_allocator<B,V>::template allocator<T>&
			    , const typename ts::static_allocator<B,V>::template allocator<U>&)
{ return false; }


