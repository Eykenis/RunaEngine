#pragma once
#include "Allocator.h"

class MemoryManager {
private:
  void* Allocate(uint32_t size);
  void Free(void* p, uint32_t size);
  MemoryManager();
  static MemoryManager manager;
public:
  virtual ~MemoryManager();
  template <typename T>
  static T* New() {
    return new(manager.Allocate(sizeof(T))) T();
  }

  template <typename T, typename... Args>
  static T* New(Args&&... args) {
    return new(manager.Allocate(sizeof(T))) T(std::forward<Args>(args)...);
  }
  
  template <typename T>
  static T* New(T t) {
    return new(manager.Allocate(sizeof(T))) T(t);
  }

  template <typename T>
  static void Delete(T* p) {
    reinterpret_cast<T*>(p)->~T();
    manager.Free(p, sizeof(T));
  }

  static MemoryManager* GetInstance() {
    return &manager;
  }
};