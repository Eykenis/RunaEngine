#include <iostream>
#include "../Runtime/Core/Memory/MemoryManager.h"
#include "../Runtime/Scene/GameObject.h"

int main()
{
  int* a = MemoryManager::GetInstance()->New<int>(5);
  GameObject gameobject;
  std::cout << "GUID: " << gameobject.GetGUID() << std::endl;
  std::cout << a << " " << *a << std::endl;
  MemoryManager::GetInstance()->Delete(a);
  std::cout << a << " " << std::hex << *a << std::endl;
  return 0;
}