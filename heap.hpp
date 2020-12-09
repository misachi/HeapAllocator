#ifndef _HEAP_H
#define _HEAP_H

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <iostream>

struct freeObject {
  struct freeObject *next;
};

class mallocHeap {
public:
  void *malloc(size_t sz) {
    // std::cout << sz << "\n";
    auto ptr = std::malloc(sz);
    if (!ptr)
      return nullptr;
    return ptr;
  }

  void free(void *ptr) {
    std::free(ptr);
  }
};

template<typename Super>
class HeaderHeap : public Super {
public:
  void *malloc(size_t sz) {
    auto *ptr = Super::malloc(sz + sizeof(size_t));
    if (!ptr)
      return nullptr;
    *(size_t *)ptr = sz;
    return (void *)((size_t *)ptr + 1);
  }

  void free(void *ptr) {
    std::memset(ptr, 0, getSize(ptr));
    Super::free((size_t *)ptr - 1);
    ptr = nullptr;
  }

  static size_t getSize(const void *ptr) {
    return *((size_t *)ptr - 1);
  }
};

template<typename Super, size_t max_small_size=256>
class FreeListHeap : public Super {
public:
  FreeListHeap() {
    freeList = nullptr;
  }
  ~FreeListHeap() {
    while(freeList) {
      freeObject *memobj = freeList;
      Super::free(memobj);
      freeList = freeList->next;
    }
    freeList = nullptr;
  }
  void *malloc(size_t sz) {
    void *ptr = nullptr;
    if (sz >= max_small_size) {
      ptr = Super::malloc(sz);
      if (!ptr)
        return nullptr;
      return ptr;
    }

    ptr = freeList;
    if (!ptr) {
      ptr = Super::malloc(sz);
      if (!ptr)
        return nullptr;
    } else {
      freeList = freeList->next;
    }
    return ptr;
  }

  void free(void *ptr) {
    size_t sz = Super::getSize(ptr);
    if (sz >= max_small_size) {
      Super::free(ptr);
    } else {
      freeObject *next = freeList;
      freeList = (freeObject *)ptr;
      freeList->next = next;
    }
  }
private:
  freeObject *freeList;
};

template<typename Super>
class LockedHeap : public Super {
public:
  void *malloc(size_t sz) {
    mtx.lock();
    void *ptr = Super::malloc(sz);
    mtx.unlock();
    if (!ptr)
      return nullptr;
    return ptr;
  }

  void free(void *ptr) {
    mtx.lock();
    Super::free(ptr);
    mtx.unlock();
  }

private:
  std::mutex mtx;
};

#endif