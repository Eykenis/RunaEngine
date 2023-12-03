#pragma once
#include "../RenderModule.h"

#include <Windows.h>
#include <cstdio>
#include <debugapi.h>
#include <dxgi.h>
#include <dxgitype.h>
#include <minwinbase.h>
#include <windowsx.h>
#include <tchar.h>
#include <minwinbase.h>
#include <winuser.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

class RenderD3D11 : public RenderModule {
public:
  virtual int init();
  virtual int release();
  virtual void clear();
  virtual void draw();
  virtual ~RenderD3D11();

  void getHwnd(HWND hwnd);
private:
  HWND m_hwnd;
};