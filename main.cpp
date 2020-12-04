#include "heap.hpp"

#include <iostream>

int main() {
  Heap heap;
  size_t sz = 30;

  for (size_t i = 0; i < 10; i++) {
    auto h1 = (size_t*)heap.malloc(sz);
    if (!h1) {
      std::cerr << "Out of memory" << "\n";
      exit(1);
    }
    *h1 = 5000;
    heap.free(h1);
  }
}
