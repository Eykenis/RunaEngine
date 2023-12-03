

# 制作自己的游戏引擎（四）内存管理

本篇讨论的是核心层的内存管理部分。这部分本来是写在 2 篇中的，但由于文章 3 的代码是先于内存管理这一段提交的，所以还是放到 4 篇来免得 git 提交和文章顺序混乱。

# 4.1 内存管理

回想起上半年找客户端实习，结果被引擎组的面试官拷打内存池是什么，从那开始才知道原来游戏是需要自己的内存管理的（

简单来说，因为 C/C++ 的内存分配接口需要陷入内核态去申请空间，所以比较慢。对于要求尽可能实时的游戏系统是很不好的。因此可以把原来发生在每一帧的内存申请与释放放到更集中的时间段统一处理，来提升系统帧率。

这就需要我们再抽象一层内存分配。比如游戏开始前先 new/malloc 一大段空间放到内存里，然后游戏自己再有一个 memory manager, 它来管理每一帧的内存分配与释放。由于 memory manager 不需要陷入内核态申请内存而是可以直接在内存上分配，因此速度比 new/malloc 快非常多。

那么怎么实现这样一个系统呢？我们可以参考操作系统的分页分段等机制。

参考这篇文章：

https://allenchou.net/2013/05/memory-management-part-3-of-3-stl-compatible-allocators/

实现的底层分配算法。

其实现的类 Allocator 将整个内存分成若干个 pages, 每个 page 有固定数量的 blocks. 而每个 block 的大小由调用者希望的申请单元大小以及系统字节对齐决定。

调用者每次申请时，都会给其分配一个 block 的内存。如果一个 page 的 blocks 被分配完了，就从内核申请内存并新建一个 page，并一个一个分配新 page 的block.

回到引擎，在 Core 下新建文件夹 Memory，再新建 Allocator.h 和 Allocator.cpp.

Allocator.h:

```cpp
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
```

Allocator.cpp:

```cpp
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
```

Allocator 实现的是 “物理” 的分配内容，其中的内存是实际使用的。为了实现分页，我们还需要一个 Memory Manager 用于抽象的页管理，这个 Manager 应该具有一个页表用于查找。

1. 一般希望这个接口是 C-like 的，和原始的 new、malloc 等相似。这样用户代码用起来方便很多。因此我们最后的 public 接口只有分配与释放两个函数
2. 对于实时系统，我们还希望它能分类处理多种不同的分配工作。例如有的 call 要求的是大片空间，但通常访问较少；有的要求空间小但访问非常频繁。如果能分开处理这多种情况，胃口大的用大 block size 的 allocator，胃口小的用 block size 小的，就能有效减少 allocator 对 blocks 的分配与释放次数，以及减小冗余空间。因此采用一个查找表，对来临的请求查找最适合它的分配策略。

参考还是之前同一个博主的代码：

http://allenchou.net/2013/05/memory-management-part-2-of-3-c-style-interface/

并且再封装一个 Memory Manager 类：

MemoryManager.h:

```cpp
#pragma once
#include "Allocator.h"

class MemoryManager {
private:
  void* Allocate(uint32_t size);
  void Free(void* p, uint32_t size);
public:
  template <typename T>
  T* New() {
    return new(Allocate(sizeof(T))) T();
  }
  
  template <typename T>
  T* New(T t) {
    return new(Allocate(sizeof(T))) T(t);
  }

  template <typename T>
  void Delete(T* p) {
    reinterpret_cast<T*>(p)->~T();
    Free(p, sizeof(T));
  }
};
```

MemoryManager.cpp:

```cpp
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

```

写一个主程序测试一下：

```cpp
#include <iostream>
#include "../Runtime/Core/Memory/MemoryManager.h"

int main()
{
  MemoryManager manager;
  int* a = manager.New<int>(5);
  std::cout << a << " " << *a << std::endl;
  manager.Delete(a);
  std::cout << a << " " << std::hex << *a << std::endl;
  return 0;
}
```

其实这里写法是不太规范的。因为 MemoryManager 是一个纯函数类，我们应该把它写成单例模式，后面再改吧。

输出：

```powershell
000001D02E7B2CB8 5
000001D02E7B2CB8 2e7b2cc0
```

保存 push 到 try4 分支.
