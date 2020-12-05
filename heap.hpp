#ifndef _HEAP_H
#define _HEAP_H

#include <cstdlib>
#include <cstring>
#include <mutex>

#define ALIGN 8
#define ALIGN_MASK (ALIGN - 1)
#define MIN_BLOCK_SIZE ALIGN
#define LARGE_BLOCK 256
#define BIN_SIZE 128
#define MIN_OVERHEAD  (sizeof(size_t))

typedef size_t bin_t;

inline size_t alignBlock(size_t sz) {
  return (sz + ALIGN_MASK) & ~ALIGN_MASK;
}

inline size_t getBlockSize(size_t sz) {
  return (sz > 0) ? (sz + MIN_OVERHEAD) : (MIN_BLOCK_SIZE + MIN_OVERHEAD);
}

class mallocHeap {
public:
  void *malloc(size_t sz) {
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
    size_t *ptr = (size_t *)Super::malloc(getBlockSize(sz));
    if (!ptr)
      return nullptr;
    *ptr = sz;
    return (void *)(ptr + 1);
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

template<typename Super, size_t minSize=24, size_t maxSize=32>
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
    if (sz < minSize || sz > maxSize) {
      void *ret = Super::malloc(sz);
      if (!ret)
        return nullptr;
      return ret;
    }
    void *ptr = freeList;

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
    if (sz < minSize || sz > maxSize) {
      Super::free(ptr);
    } else {
      freeObject *next = freeList;
      freeList = (freeObject *)ptr;
      freeList->next = next;
    }
  }
private:
  class freeObject {
  public:
    freeObject *next;
  };
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

template<typename Super, typename FList, bin_t binNum>
class SegHeapList: public Super {
public:
  void *malloc(size_t sz) {

  }
private:
  FList segList[binNum];
};

typedef
LockedHeap<FreeListHeap<HeaderHeap<mallocHeap>>> Heap;

#endif