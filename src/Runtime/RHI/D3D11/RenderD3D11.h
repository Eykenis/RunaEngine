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

void CreateRenderTarget();
void SetViewPort();
HRESULT CompileShader(const char *srcData, LPCSTR profile, ID3DBlob* pShaderBlob) ;
BOOL InitPipeline();
void InitGraphics();
HRESULT CreateGraphicsResources(HWND hwnd);
void DiscardGraphicsResources();
void RenderFrame();