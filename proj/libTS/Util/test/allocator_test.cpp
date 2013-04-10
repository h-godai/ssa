
#include <stdio.h>
#include "../allocator.hpp"

using namespace ts::mem;

namespace {
  struct test_allocator {
    typedef uint32_t size_type;
    static void* allocate(size_type n) {
      printf("ALLOCATE %u\n", n);
      return new uint8_t[n];
    }
    static void deallocate(void* ptr) {
      printf("DEALLOCATE %p\n", ptr);
      delete [] reinterpret_cast<uint8_t*>(ptr);
    }
  };
}

int main(int ac, char* av[]) {

  typedef allocator<256       // element size
                  , 0         // hint
                  , test_allocator   // allocator
                  , no_mutex
                  , 4  // align
                  , 32 // nextsize
                  , 32 // maxsize
    > alc; 
  
  alc a;
  const size_t arsz = 34;
  void* ptrs[arsz];
  size_t total = 0;
  for (size_t n = 0; n < arsz; ++n) {
    ptrs[n] = a.malloc();
    total += 256;
    printf("malloc %p total %lu\n", ptrs[n], total);
  }
}

