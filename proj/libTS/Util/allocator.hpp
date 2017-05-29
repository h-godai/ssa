// allocator
// if compile on Visual Studio, add "stdint.h" to include path
#include <stdint.h>
#include <algorithm>
#include <limits>


#include "pool/pool_storage.hpp"

namespace ts { namespace pool {
  typedef std::size_t size_type;

  // 与えられたバッファを使うシンプルなアロケーター
  template <size_type Alignment = 8>
  struct simple_allocator {
    struct free_chunk {
      free_chunk* next_;
      free_chunk* prev_;
      size_type buffer_size_;  // buffer bytes
      bool allocated_;
      uint8_t padding_[alignment<Alignment, sizeof(free_chunk*)*2 + sizeof(size_type) + sizeof(bool)>::padding]; // padding
      uint8_t buffer_[];
      free_chunk()
        : next_(0)
        , prev_(0)
        , buffer_size_(0)
        , allocated_(false)
      {}
    };

    free_chunk* root_chunk_;
    free_chunk* last_chunk_;
    bool need_delete_;
    size_type buffer_size_;

    simple_allocator(void* buffer, size_type size)
      : root_chunk_(reinterpret_cast<free_chunk*>(buffer))
      , last_chunk_(root_chunk_)
      , need_delete_(false)
      , buffer_size_(size)
    {
      root_chunk_->buffer_size_ = size - sizeof(free_chunk);
      root_chunk_->next_ = 0;
      root_chunk_->prev_ = 0;
      root_chunk_->allocated_ = false;
    }
    simple_allocator(size_type size)
      : root_chunk_(reinterpret_cast<free_chunk*>(new uint8_t[size]))
      , last_chunk_(root_chunk_)
      , need_delete_(true)
      , buffer_size_(size)
    {
      root_chunk_->buffer_size_ = size - sizeof(free_chunk);
      root_chunk_->next_ = 0;
      root_chunk_->prev_ = 0;
      root_chunk_->allocated_ = false;
    }
    ~simple_allocator() {
      if (need_delete_) {
        delete [] (reinterpret_cast<uint8_t*>(root_chunk_));
      }
    }

    void* malloc(size_type n) {
      free_chunk* ch = last_chunk_;
      bool around = false;
      for (;;) {
        if (!ch->allocated_ && ch->buffer_size_ >= n) {
          return use_chunk(ch, n);
        }
        ch = ch->next_;
        if (ch == last_chunk_) return 0; // out of memory
        if (ch == 0) {
          ch = root_chunk_;
		  //          if (around) throw std::bad_alloc("invalid last chunk");
		  if (around) throw std::bad_alloc();
          around = true;
        }
      }
    }
    void free(void* ptr) {
      if (ptr == 0) return;
      free_chunk* free = reinterpret_cast<free_chunk*>(reinterpret_cast<uint8_t*>(ptr) - sizeof(free_chunk));
      free->allocated_ = false;
      if (free->prev_ != 0 && !free->prev_->allocated_) {
        free = marge_chunk(free->prev_, free);
      }
      if (free->next_ != 0 && !free->next_->allocated_) {
        marge_chunk(free, free->next_);
      }
      last_chunk_ = free;
    }

    void purge_memory() {
      last_chunk_ = root_chunk_;
      root_chunk_->buffer_size_ = buffer_size_ - sizeof(free_chunk);
      root_chunk_->next_ = 0;
      root_chunk_->prev_ = 0;
      root_chunk_->allocated_ = false;
    }

  private:
    void* use_chunk(free_chunk* ch, size_type allocbytes) {
      // 空いているエリアが小さい場合はそのまま割り当てる //
      if (ch->buffer_size_ < allocbytes + sizeof(free_chunk) + Alignment) {
        ch->allocated_ = true;
        return ch;
      }
      allocbytes = alignment<Alignment>::alignment_size(allocbytes);

      // 隣接する使用領域の小さい方にオフセットして確保する //
      size_type upper_size = (ch->next_ != 0) ? ch->next_->buffer_size_ : 0;
      size_type lower_size = (ch->prev_ != 0) ? ch->prev_->buffer_size_ : 0;

      // チャンクを分割 //
      return split_chunk(ch, allocbytes, (upper_size < lower_size));
    }
    void* split_chunk(free_chunk* ch, size_type allocbytes, bool upperside) {
//      size_type chunksize = ch->buffer_size_;
      size_type newchunksize = allocbytes + sizeof(free_chunk);
      if (upperside) {
        // チャンクの後半をアロケート //
        ch->buffer_size_-= newchunksize;
        free_chunk* ptr = reinterpret_cast<free_chunk*>(&ch->buffer_[ch->buffer_size_]);
        ptr->next_ = ch->next_;
        ch->next_ = ptr;
        ptr->buffer_size_ = allocbytes;
        ptr->prev_ = ch;
        ptr->allocated_ = true;
 //       assert(ch->buffer_size_ + ptr->buffer_size_ + sizeof(free_chunk) == chunksize);
        return &ptr->buffer_[0];
      }
      else {
        // チャンクの前半をアロケート //
        free_chunk* ptr = reinterpret_cast<free_chunk*>(&ch->buffer_[allocbytes]);
        ptr->buffer_size_ = ch->buffer_size_ - newchunksize;
        ptr->next_ = ch->next_;
        ptr->prev_ = ch;
        ptr->allocated_ = false;
        ch->buffer_size_ = allocbytes;
        ch->next_ = ptr;
        ch->allocated_ = true;
        last_chunk_ = ptr;
 //       assert(ch->buffer_size_ + ptr->buffer_size_ + sizeof(free_chunk) == chunksize);
        return &ch->buffer_[0];
      }
    }

    free_chunk* marge_chunk(free_chunk* a, free_chunk* b) {
      a->buffer_size_ += b->buffer_size_ + sizeof(free_chunk);
      a->next_ = b->next_;
      if (b->next_ != 0) b->next_->prev_ = a;
      return a;
    }


   

  };





}}


