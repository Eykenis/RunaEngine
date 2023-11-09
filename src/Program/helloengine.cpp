#include <iostream>
#include "../Runtime/Core/eigen-3.4.0/Eigen/Eigen"

int main()
{
  printf("Hello, Runa Engine!");
  Eigen::Matrix4f mat1 = Eigen::Matrix4f::Zero();
  Eigen::Matrix4f mat2 = Eigen::Matrix4f::Zero();

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      std::cin >> mat1(i, j);
    }
  }

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      std::cin >> mat2(i, j);
    }
  }

  std::cout << mat1 * mat2 << std::endl;

  return 0;
}