#ifndef TS_UTIL_POOL_UTIL_HPP
#define TS_UTIL_POOL_UTIL_HPP

namespace ts { namespace pool {

  typedef std::size_t size_type;

    template <size_type Align, size_type ElementSize=1>
    struct alignment {
      static const size_type blocks = (ElementSize + Align - 1)/Align;
      static const size_type size = Align * blocks;
      static const size_type padding = size - ElementSize;
      static size_type alignment_size(size_type const n) {
        return Align * ((n + Align - 1) / Align);
      }
    };

    struct new_delete_allocator {
      typedef size_t size_type;
      static void* allocate(size_type n) {
	return new uint8_t[n];
      }
      static void deallocate(void* ptr) {
	delete [] reinterpret_cast<uint8_t*>(ptr);
      }
    };

    struct no_mutex {
      static void lock() {}
      static void unlock() {}
    };

}}

#endif
