#include "Allocator.h"

void Allocator::Reset(uint32_t dataSize,
    uint32_t pageSize,
    uint32_t alignment) {
      FreeAll();

      m_dataSize = dataSize;
      m_pageSize = pageSize;
      m_alignmentSize = alignment;

      uint32_t maxHeaderData = std::max(sizeof(BlockHeader), (unsigned long long)m_dataSize);
      m_alignmentSize = maxHeaderData % alignment ? alignment - maxHeaderData % alignment : 0;
      m_blockSize = maxHeaderData + m_alignmentSize;
      m_blocksPerPage = (m_pageSize - sizeof(PageHeader)) / m_blockSize;
    }

Allocator::Allocator(uint32_t dataSize,
    uint32_t pageSize,
    uint32_t alignment)
    : m_pageList(nullptr), m_freeList(nullptr) {
  Reset(dataSize, pageSize, alignment);
}

Allocator::Allocator() {

}

Allocator::~Allocator() {
  FreeAll();
}

void* Allocator::Allocate(void) {
  if (!m_freeList) {
    // set new page
    PageHeader* newPage = reinterpret_cast<PageHeader*>(new char[m_pageSize]);
    ++m_numPages;
    m_numBlocks += m_blockSize;
    m_numFreeBlocks += m_blockSize;
    FillFreePage(newPage);

    // insert into head
    if (m_pageList)
      newPage->pNext = m_pageList;
    m_pageList = newPage;

    BlockHeader* curBlock = newPage->Blocks();
    for (unsigned i = 0; i < m_blocksPerPage - 1; ++i) {
      curBlock->pNext = NextBlock(curBlock);
      curBlock = NextBlock(curBlock);
    }
    curBlock->pNext = nullptr;
    m_freeList = newPage->Blocks();
  }
  BlockHeader* freeBlock = m_freeList;
  m_freeList = m_freeList->pNext;
  m_numFreeBlocks--;

  FillAllocatedBlock(freeBlock);
  return freeBlock;
}

void Allocator::Free(void *p) {
  BlockHeader* block = reinterpret_cast<BlockHeader*>(p);
  FillFreeBlock(block);
  block->pNext = m_freeList;
  m_freeList = block;
  m_numFreeBlocks++;
}

void Allocator::FreeAll() {
  PageHeader* pageWalker = m_pageList;
  while (pageWalker) {
    PageHeader *curPage = pageWalker;
    pageWalker = pageWalker->pNext;
    delete[] reinterpret_cast<char*>(curPage);
  }
  m_pageList = nullptr;
  m_freeList = nullptr;
  m_numPages = 0;
  m_numFreeBlocks = 0;
  m_numBlocks = 0;
}

void Allocator::FillFreePage(PageHeader* p) {
  p->pNext = nullptr;
  BlockHeader* curBlock = p->Blocks();
  for (unsigned i = 0; i < m_blocksPerPage; ++i) {
    FillFreeBlock(curBlock);
    curBlock = NextBlock(curBlock);
  }
}

void Allocator::FillFreeBlock(BlockHeader* p) {
  memset(p, PATTERN_FREE, m_blockSize - m_alignmentSize);
  memset(reinterpret_cast<char*>(p) + m_blockSize - m_alignmentSize, PATTERN_ALIGN, m_alignmentSize);
}

void Allocator::FillAllocatedBlock(BlockHeader* p) {
  memset(p, PATTERN_ALLOC, m_blockSize - m_alignmentSize);
  memset(reinterpret_cast<char*>(p) + m_blockSize - m_alignmentSize, PATTERN_ALIGN, m_alignmentSize);
}

BlockHeader* Allocator::NextBlock(BlockHeader* p) {
  // return p + 1;
  return reinterpret_cast<BlockHeader*>(reinterpret_cast<char*>(p) + m_blockSize);
}