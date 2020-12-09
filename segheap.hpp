#ifndef _SEG_HEAP
#define _SEG_HEAP

#include "heap.hpp"

/*
Segemented list with bins that are 8bytes apart and hold chunks of memory of minimum size CHUNK_SIZE. 

*/

#define ALIGN 8
#define SMALL_SHIFT 3
#define ALIGN_MASK (ALIGN - 1)
#define MAX_SMALL_BLOCK 512
#define BIN_SIZE 64

#define CHUNK_SIZE (sizeof(freeObject))
#define MIN_CHUNK_SIZE ((CHUNK_SIZE + ALIGN_MASK) & ~ALIGN_MASK)

typedef unsigned long bin_t;

static inline bin_t getBinClass(size_t sz) {
  int idx = (sz >> SMALL_SHIFT) - (MIN_CHUNK_SIZE >> SMALL_SHIFT);
  if (idx <= 0)
    return 0;
  return idx;
}

static inline bool alignOk(size_t sz) {
  return (sz & ALIGN_MASK) == 0;
}

static inline size_t alignBlock(size_t sz) {
  return (alignOk(sz)) ? sz : ((sz + ALIGN_MASK) & ~ALIGN_MASK);
}

static inline size_t getBlockSize(size_t sz) {
  return (sz < CHUNK_SIZE) ? MIN_CHUNK_SIZE : alignBlock(sz);
}

template<typename Super, typename FList, typename Tag, bin_t binNum>
class SegHeapList: public Super {
public:
  void *malloc(size_t sz) {
    void *ptr {nullptr};
    sz = getBlockSize(sz);
    bin_t bin_idx {getBinClass(sz)};

    if (bin_idx >= BIN_SIZE) {
      ptr = Super::malloc(sz);
      if (!ptr)
        return nullptr;
      return ptr;
    } else {
      FList &bin_list {segList[bin_idx]};
      ptr = bin_list.malloc(sz);
      if (!ptr)
        return nullptr;
      return ptr;
    }
  }

  void free(void *ptr) {
    size_t sz {Super::getSize(ptr)};
    bin_t bin_idx = getBinClass(sz);
    if (bin_idx >= BIN_SIZE) {
      Super::free(ptr);
    } else {
      FList &bin_list {segList[bin_idx]};
      bin_list.free(ptr);
    }
  }
private:
  FList segList[binNum];
};

#endif