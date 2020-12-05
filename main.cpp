#include "heap.hpp"

#include <iostream>
#include <string>

#define MEM_ALLOC_ERR_MSG "Out of memory"

static inline void malloc_err_debug(void *ptr, std::string msg) { if (!ptr) {
    std::cerr << msg << "\n";
    exit(1);
  }
}

int main() {
  Heap heap;
  size_t sz = sizeof(size_t);
  auto h1 = (size_t*)heap.malloc(sz);
  malloc_err_debug(h1, MEM_ALLOC_ERR_MSG);
  *h1 = 5000;

  auto h2 = (size_t*)heap.malloc(sz);
  malloc_err_debug(h2, MEM_ALLOC_ERR_MSG);
  *h2 = 2098;

  std::cout << h1 << "\n";
  std::cout << h2 << "\n";
  heap.free(h2);
  heap.free(h1);

  auto h3 = (size_t*)heap.malloc(sz);
  malloc_err_debug(h3, MEM_ALLOC_ERR_MSG);
  *h2 = 2098;

  std::cout << h3 << "\n";
  heap.free(h3);
}
