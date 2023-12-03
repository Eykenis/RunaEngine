#include "../Runtime/Core/eigen-3.4.0/Eigen/Eigen"
#include "../Runtime/Forms/Win32/FormWin.h"
#include <iostream>

int main()
{
  Form* form = new FormWin();
  std::cout << "Select API you want. 1 for OpenGL, 2 for DirectX 11." << std::endl;
  int api; std::cin >> api;
  form->InitForm(640, 480, "Runa Engine", api);
  while (form->DisplayFrame(0)) ;
  return 0;
}