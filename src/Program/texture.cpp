#include "../Runtime/AssetsImport/AssetsManager.h"
#include "../Runtime/RHI/D3D11/RenderD3D11.h"
#include <d3d11.h>

int main() {
  AssetsManager loader;
  ImageBufferHeader* img = loader.ReadImage("../assets/Images/test.png");

  
}