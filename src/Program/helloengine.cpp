#include "../Runtime/Core/eigen-3.4.0/Eigen/Eigen"
#include "../Runtime/Forms/Win32/FormWin.h"

int main()
{
  Form* form = new FormWin();
  form->InitForm(640, 480, "Runa Engine");
  while (form->DisplayFrame(0)) ;
  return 0;
}