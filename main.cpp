#include "segheap.hpp"
#include "heap.hpp"

#include <iostream>
#include <string>

#define MALLOC_ERR_MSG "Out of memory"

// typedef
// LockedHeap<FreeListHeap<HeaderHeap<mallocHeap>, MAX_SMALL_BLOCK>> Heap;

typedef FreeListHeap<HeaderHeap<mallocHeap>> FList;

typedef SegHeapList<HeaderHeap<mallocHeap>, LockedHeap<FList>, struct chunk, BIN_SIZE> Heap;

static inline void malloc_err_debug(void *ptr, std::string msg) { 
  std::cout << ptr << "\n";
  if (!ptr) {
    std::cerr << msg << "\n";
    exit(1);
  }
}

int main() {
  Heap heap;
  size_t sz = sizeof(size_t);
  auto h1 = (size_t*)heap.malloc(sizeof(size_t));
  malloc_err_debug(h1, MALLOC_ERR_MSG);
  *h1 = 5000;

  auto h2 = (size_t *)heap.malloc(sizeof(size_t));
  malloc_err_debug(h2, MALLOC_ERR_MSG);
  *h2 = 10;

  heap.free(h2);
  heap.free(h1);

  auto h3 = (size_t *)heap.malloc(sizeof(size_t));
  malloc_err_debug(h3, MALLOC_ERR_MSG);
  *h3 = 20;

  auto h4 = (size_t *)heap.malloc(sizeof(size_t)*300);
  malloc_err_debug(h4, MALLOC_ERR_MSG);
  *h4 = 20;

  heap.free(h3);
  heap.free(h4);
}
