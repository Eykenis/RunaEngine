#pragma once
#include "Allocator.h"

class MemoryManager {
private:
  void* Allocate(uint32_t size);
  void Free(void* p, uint32_t size);
public:
  MemoryManager();
  virtual ~MemoryManager();
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