#ifndef _HEAP_H
#define _HEAP_H

#include <cstdlib>
#include <cstring>
#include <mutex>

#define MINBLOCKSIZE 4
#define LARGEBLOCK 256
#define BINSIZE 128

inline size_t getBlockSize(size_t sz) {
  if(sz <= 0) {
    return (MINBLOCKSIZE+sizeof(size_t));
  }
  return (sz+sizeof(size_t));
}

typedef size_t bin_t;

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

typedef LockedHeap<FreeListHeap<HeaderHeap<mallocHeap>>> Heap;

#endif