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