#include "MemoryManager.h"

static const uint32_t k_blockSizes[] = {
  // 4-increments
4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 

// 32-increments
128, 160, 192, 224, 256, 288, 320, 352, 384, 
416, 448, 480, 512, 544, 576, 608, 640, 

// 64-increments
704, 768, 832, 896, 960, 1024
};

static const uint32_t k_pageSize = 4096; // or 8192
static const uint32_t k_alignment = 4;

static const uint32_t k_numBlockSizes = sizeof(k_blockSizes) / sizeof(k_blockSizes[0]);
static const uint32_t k_maxBlockSize = k_blockSizes[k_numBlockSizes - 1];

static uint32_t *s_blockSizeLookup = nullptr;
static Allocator s_allocators[k_numBlockSizes];

static Allocator* LookUpAllocator(uint32_t size) {
  // static is one-time
  static bool s_initialized = false;
  if (!s_initialized) {
    s_blockSizeLookup = new uint32_t[k_maxBlockSize + 1];
    uint32_t j = 0;
    for (uint32_t i = 0; i <= k_maxBlockSize; ++i) {
      if (i > k_blockSizes[j]) j++;
      s_blockSizeLookup[i] = j; // to find which size
    }
    for (uint32_t i = 0; i < k_numBlockSizes; ++i) {
      s_allocators[i].Reset(k_blockSizes[i], k_pageSize, k_alignment);
    }
    s_initialized = true;
  }
    if (size <= k_maxBlockSize) return s_allocators + s_blockSizeLookup[size];
    return nullptr;
}

void* MemoryManager::Allocate(uint32_t size) {
  Allocator* alloc = LookUpAllocator(size);
  if (alloc) return alloc->Allocate();
  else return malloc(size); // if failed don't just crash, use std
}

void MemoryManager::Free(void* p, uint32_t size) {
  Allocator* alloc = LookUpAllocator(size);
  if (alloc) return alloc->Free(p);
  return free(p);
}
