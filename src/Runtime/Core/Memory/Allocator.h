#pragma once
#include <cstdint>
#include <string>
#include <algorithm>
#include <malloc.h>

struct BlockHeader {
  BlockHeader *pNext;
};

struct PageHeader {
  PageHeader *pNext;
  BlockHeader *Blocks() {
    return reinterpret_cast<BlockHeader*>(this + 1);
  }
};

class Allocator {
private:
  void FillFreePage(PageHeader *p);
  void FillFreeBlock(BlockHeader *p);
  void FillAllocatedBlock(BlockHeader *p);
  BlockHeader* NextBlock(BlockHeader *p);

  PageHeader* m_pageList;
  BlockHeader* m_freeList;

  uint32_t m_dataSize;
  uint32_t m_pageSize;
  uint32_t m_alignmentSize;
  uint32_t m_blockSize;
  uint32_t m_blocksPerPage;

  uint32_t m_numPages;
  uint32_t m_numBlocks;
  uint32_t m_numFreeBlocks;

  // disable copy and assignment op
  Allocator(const Allocator &clone);
  Allocator &operator=(const Allocator &rhs);
public:
  static const unsigned char PATTERN_ALIGN = 0xFC;
  static const unsigned char PATTERN_ALLOC = 0xFD;
  static const unsigned char PATTERN_FREE  = 0xFE;
  
  Allocator (
    uint32_t dataSize,
    uint32_t pageSize,
    uint32_t alignment
  );

  // do nothing
  Allocator();

  virtual ~Allocator();

  void Reset(
    uint32_t dataSize,
    uint32_t pageSize,
    uint32_t alignment
  ); // data should be aligned by %alignment% bytes.

  void* Allocate(void);

  void Free(void *pToFree);

  void FreeAll();
};