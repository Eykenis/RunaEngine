#include "RenderD3D11.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include <dxgiformat.h>
#include <intsafe.h>
#include <minwinbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <wrl.h>

#include "../../AssetsImport/AssetsManager.h"

const uint32_t SCREEN_WIDTH = 640;
const uint32_t SCREEN_HEIGHT = 480;
const uint32_t REFRESH_RATE = 60;

// global variables
IDXGISwapChain *g_pSwapChain = nullptr;
ID3D11Device *g_pDevice = nullptr;
ID3D11DeviceContext *g_pDeviceContext = nullptr;
ID3D11RenderTargetView *g_pRTView = nullptr;
ID3D11InputLayout *g_pLayout = nullptr;
ID3D11VertexShader *g_pVS = nullptr;
ID3D11PixelShader *g_pPS = nullptr;
ID3D11Buffer* g_pVBuffer = nullptr;

ID3D11ShaderResourceView *g_pDiffuseSRV = nullptr;
ID3D11SamplerState *g_pImageSamplerState;

struct VERTEX {
  XMFLOAT3 Position;
  XMFLOAT2 UV;
};

VERTEX OutputVertices[] = {
  {XMFLOAT3{0.0f, 0.5f, 0.0f}, XMFLOAT2(0.5f, 1.0f)},
  {XMFLOAT3{0.45f, -0.5f, 0.0f}, XMFLOAT2(1.0f, 0.0f)},
  {XMFLOAT3{-0.45f, -0.5f, 0.0f}, XMFLOAT2(0.0f, 0.0f)},
};


template<class T>
inline void SafeRelease(T **pToRelease) {
  if (pToRelease != nullptr) (*pToRelease)->Release();
  *pToRelease = nullptr;
}

void CreateRenderTarget() {
  HRESULT hr;
  ID3D11Texture2D *pBackBuffer;

  hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
  assert(SUCCEEDED(hr));

  hr = g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTView);
  assert(SUCCEEDED(hr));

  pBackBuffer->Release();

  g_pDeviceContext->OMSetRenderTargets(1, &g_pRTView, NULL);
}

void SetViewPort() {
  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = SCREEN_WIDTH;
  viewport.Height = SCREEN_HEIGHT;
  
  g_pDeviceContext->RSSetViewports(1, &viewport);
}

HRESULT CompileShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR profile, ID3DBlob **pShaderBlob) {
  ID3DBlob *pErrBlob, *pTmpBlob;
  HRESULT hr;

  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
  #if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
  #endif

  const D3D_SHADER_MACRO defines[] = 
  {
      "EXAMPLE_DEFINE", "1",
      NULL, NULL
  };

  hr = D3DCompileFromFile(
    fileName,
    defines,
    D3D_COMPILE_STANDARD_FILE_INCLUDE,
    entryPoint,
    profile,
    flags,
    0,
    &pTmpBlob,
    &pErrBlob
  );

  if (FAILED(hr)) {
    if (pErrBlob) {
      printf("Shader Compile Error: %s", (char*)pErrBlob->GetBufferPointer());
      pErrBlob->Release();
    }
    if (pTmpBlob) {
      pTmpBlob->Release();
    }
    return hr;
  }
  *pShaderBlob = pTmpBlob;

  return hr;
}

BOOL InitPipeline() {
  ID3DBlob *pVSBlob, *pPSBlob, *pErrBlob;
  HRESULT hr;
  // compile shaders
  hr = CompileShader(L"../src/Runtime/RHI/D3D11/shader.vs", "VSMain", "vs_5_0", &pVSBlob);
  if (FAILED(hr)) {
    printf("Failed compiling vertex shader: %08lX\n", hr);
    return FALSE;
  }
  hr = CompileShader(L"../src/Runtime/RHI/D3D11/shader.ps", "PSMain", "ps_5_0", &pPSBlob);
  if (FAILED(hr)) {
    printf("Failed compiling pixel shader: %08lX\n", hr);
    return FALSE;
  }

  // set to device shader
  hr = g_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVS);
  assert(SUCCEEDED(hr));
  hr = g_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPS);
  assert(SUCCEEDED(hr));

  // create input layout
  D3D11_INPUT_ELEMENT_DESC ied[] = 
  {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  hr = g_pDevice->CreateInputLayout(ied, ARRAYSIZE(ied), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pLayout);
  assert(SUCCEEDED(hr));

  g_pDeviceContext->IASetInputLayout(g_pLayout);

  pVSBlob->Release();
  pPSBlob->Release();

  return TRUE;
}

void InitGraphics()
{
  HRESULT hr;
  // create vertex buffer (for GPU use)
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(OutputVertices);
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

  D3D11_SUBRESOURCE_DATA vertexDataInitial = { OutputVertices };

  hr = g_pDevice->CreateBuffer(&bd, &vertexDataInitial, &g_pVBuffer);
  assert(SUCCEEDED(hr));

  // Create Texture
  // **********

  AssetsManager loader;
  ImageBufferHeader* image = reinterpret_cast<ImageBufferHeader*>(loader.LoadAsset("../assets/Images/test.png"));
  uint32_t imagePitch = (image->width << 2u);

  ID3D11Texture2D *pTex;
  D3D11_TEXTURE2D_DESC desc;

  desc.Width = image->width;
  desc.Height = image->height;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA initData;
  initData.pSysMem = image->data;
  initData.SysMemPitch = imagePitch;

  g_pDevice->CreateTexture2D(&desc, &initData, &pTex);
  free(image->data);

  g_pDevice->CreateShaderResourceView(pTex, nullptr, &g_pDiffuseSRV);

  const FLOAT borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  CD3D11_SAMPLER_DESC imageSamplerDesc(
    D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    D3D11_TEXTURE_ADDRESS_CLAMP,
    D3D11_TEXTURE_ADDRESS_CLAMP,
    D3D11_TEXTURE_ADDRESS_CLAMP,
    0.0f,
    1,
    D3D11_COMPARISON_NEVER,
    borderColor,
    -FLT_MAX,
    FLT_MAX
  );

  g_pDevice->CreateSamplerState(&imageSamplerDesc, &g_pImageSamplerState);

  // **********
}

HRESULT CreateGraphicsResources(HWND hwnd) {
  HRESULT hr = S_OK;
  if (g_pSwapChain == nullptr) {
    // create swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

    sd.BufferCount = 1;
    sd.BufferDesc.Width = SCREEN_WIDTH;
    sd.BufferDesc.Height = SCREEN_HEIGHT;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = REFRESH_RATE;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 4;
    sd.Windowed = TRUE;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL FeatureLevel;

    // create swapchain, device and device context.
    hr = D3D11CreateDeviceAndSwapChain(NULL,
                                      D3D_DRIVER_TYPE_HARDWARE,
                                      NULL,
                                      0,
                                      &FeatureLevels,
                                      1,
                                      D3D11_SDK_VERSION,
                                      &sd,
                                      &g_pSwapChain,
                                      &g_pDevice,
                                      &FeatureLevel,
                                      &g_pDeviceContext);
    if (FAILED(hr)) {
      return hr;
    }
    CreateRenderTarget();
    SetViewPort();
    InitPipeline();
    InitGraphics();
  }
  return hr;
}

void DiscardGraphicsResources() {
  SafeRelease(&g_pLayout);
  SafeRelease(&g_pVS);
  SafeRelease(&g_pPS);
  SafeRelease(&g_pVBuffer);
  SafeRelease(&g_pSwapChain);
  SafeRelease(&g_pRTView);
  SafeRelease(&g_pDevice);
  SafeRelease(&g_pDeviceContext);
}

/* only do this in frame update */
void RenderFrame() {

  SetViewPort();

  g_pDeviceContext->OMSetRenderTargets(1, &g_pRTView, NULL);

  const FLOAT clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
  g_pDeviceContext->ClearRenderTargetView(g_pRTView, clearColor);
  UINT stride = sizeof(VERTEX);
  UINT offset = 0;
  g_pDeviceContext->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);
  // set organization format of data (triangle)
  g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  // set input layout
  g_pDeviceContext->IASetInputLayout(g_pLayout);
  // select shaders
  g_pDeviceContext->VSSetShader(g_pVS, NULL, 0);
  g_pDeviceContext->PSSetShader(g_pPS, NULL, 0);
  // set shader resources
  g_pDeviceContext->PSSetShaderResources(0, 1, &g_pDiffuseSRV);
  g_pDeviceContext->PSSetSamplers(0, 1, &g_pImageSamplerState);
  // pick next 3 vertices and draw!
  g_pDeviceContext->Draw(3, 0);
  // swap buffers
  g_pSwapChain->Present(0, 0);
}