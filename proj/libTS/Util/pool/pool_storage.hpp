
#ifndef TS_UTIL_POOL_POOL_STORAGE_HPP
#define TS_UTIL_POOL_POOL_STORAGE_HPP


#include "../linked_list.hpp"
#include "pool_util.hpp"

namespace ts { namespace pool {

  using ts::util::linked_list;

  template <size_type ElementSize
	    , typename Allocator
	    , size_type Alignment
	    , size_type NextSize
	    , size_type MaxSize
	      >
  class pool_storage {
    typedef std::size_t size_type;

    struct chunk : linked_list<chunk> {
      union {
        chunk* next_;
        uint8_t body_[ElementSize];
      };

      void* alloc_orderd(size_type n, chunk* newchunk) {
        chunk* ch = this;
        while (ch != 0) {
          if (--n == 0) {
            *newchunk = ch->next_;
            return this;
          }
          if (ch->next_ == 0 || &ch->body_[1] != &ch->next_->body_[0]) {
            return 0;
          }
          ch = ch->next_;
        }
	return 0;
      }
    };

    struct allocate_block : linked_list<allocate_block> {
      allocate_block* next_;
      size_type size_;
      chunk chunk_[];
      bool is_from(const void* const ptr) const {
        return ptr > this && ptr < reinterpret_cast<const uint8_t*>(&chunk_[0]) + size_;
      }
    };

    static const size_type allocate_size =  alignment<Alignment, ElementSize>::size;
    allocate_block* first_;
    chunk*   freechunk_;      // free chunk 
	Allocator allocator_;
  public:
    pool_storage(const Allocator& a = Allocator())
	: first_(0), freechunk_(0), allocator_(a)
    {}
	~pool_storage() {
		purge_memory();
	}

    void* malloc() {
      if (freechunk_ == 0) freechunk_ = create_chunk(NextSize);
      if (freechunk_ == 0) return 0; // out of memory
      void* ptr = freechunk_;
      freechunk_ = freechunk_->next_;
      return ptr;
    }
    void* ordered_malloc(size_type n) {
      return get_orderd(n);
    }
    void free(void* ptr) {
      // 削除した場所をフリーチャンクの先頭にする
      chunk* ch = reinterpret_cast<chunk*>(ptr);
      chunk* oldfree_ = freechunk_;
      freechunk_ = ch;
      freechunk_->next_ = oldfree_; 
    }
    void ordered_free(void* ptr, size_type n) {
      free(ptr);
      chunk* ch = reinterpret_cast<chunk*>(ptr);
      chunk* next = ch->next_; // 従来の次のフリー
      while (--n != 0) {
        ch->next_ = &ch[1];
        ch = ch->next_;
      }
      ch->next_ = next;
    }

	void purge_memory() {

		for (allocate_block* ab = first_; ab !=0 ; ) {
			allocate_block* a = ab;
			ab = ab->next_;
			allocator_.deallocate(a);
		}
		first_ = 0;
        freechunk_ = 0;
	}




  private:
    
    void* get_orderd(size_t n = 1) {
      if (freechunk_ == 0) freechunk_ = create_chunk(std::max(n, NextSize));
      if (freechunk_ == 0) return 0;
      chunk* newchunk = 0;
      void* ptr = freechunk_->alloc_orderd(n, newchunk);
      if (ptr != 0) {
        freechunk_ = newchunk;
        return ptr;
      }
      freechunk_ = create_chunk(std::max(n, NextSize));
      if (freechunk_ == 0) return 0;
      ptr = freechunk_;
      freechunk_ = freechunk_->next(n);
      return ptr;
    }

    chunk* create_chunk(size_type n) {
      size_type allocbytes = n * allocate_size;
      allocate_block* ai = reinterpret_cast<allocate_block*>(allocator_.allocate(sizeof(allocate_block) + allocbytes));
      if (ai == 0) return 0;
      ai->size_ = allocbytes;
      for (size_t i = 0; i < n-1; ++i) {
        ai->chunk_[i].next_ = &ai->chunk_[i+1];
      }
      ai->chunk_[n-1].next_ = 0;
      ai->next_ = first_;
      first_  = ai;
      return &ai->chunk_[0];
    }

	  
  };


}} // namespace
#endif

