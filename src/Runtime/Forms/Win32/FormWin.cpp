#include "FormWin.h"
#include "../../RHI/D3D11/RenderD3D11.h"
#include <dxgi.h>
#include <winuser.h>

extern IDXGISwapChain* g_pSwapChain;


LRESULT CALLBACK myWndProc(
  HWND hwnd,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
);

void FormWin::InitForm(int formWidth, int formHeight, std::string title) {

  ATOM atom;
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.lpfnWndProc = (WNDPROC)myWndProc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpszClassName = title.c_str();
  atom = RegisterClassEx(&wc);
  assert(atom != 0);
  
  h_window = CreateWindowEx(0,
    title.c_str(), title.c_str(),
    WS_OVERLAPPEDWINDOW,
    0, 0, formWidth, formHeight, NULL, NULL, GetModuleHandle(NULL), NULL
  );
  assert(h_window != NULL);

  ShowWindow(h_window, SW_SHOW);

  return;
}

void FormWin::DestroyForm() {

}

int FormWin::DisplayFrame(unsigned char* buffer) {
  if (!buffer) {
    if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) return 0;
    if(GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      return 1;
    }
  }
  return 0;
}


LRESULT CALLBACK myWndProc(
  HWND hwnd,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
) {
  LRESULT hr = 0;
  switch (message)
  {
    case WM_PAINT:
    hr = CreateGraphicsResources(hwnd);
    RenderFrame();
    case WM_SIZE:
    if (g_pSwapChain != nullptr) {
      DiscardGraphicsResources();
    }
    break;
    case WM_DESTROY:
    DiscardGraphicsResources();
    PostQuitMessage(0);
    break;
    case WM_DISPLAYCHANGE:
    InvalidateRect(hwnd, nullptr, false);
    default: return DefWindowProc(hwnd, message, wparam, lparam);
  }
  return hr;
}