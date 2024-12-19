#include "FormWin.h"
#include "../../RHI/D3D11/RenderD3D11.h"
#include "../../RHI/OpenGL/RenderGL.h"
#include "../../RHI/OpenGL/GraphicsManagerGL.h"
#include <cstddef>
#include <libloaderapi.h>
#include <winuser.h>

HDC m_hdc;

LRESULT CALLBACK myWndProc(
  HWND hwnd,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
);

void FormWin::InitForm(int formWidth, int formHeight, std::string title, int rhi) {

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
  
  // h_window = CreateWindowEx(0,
  //   title.c_str(), title.c_str(),
  //   WS_OVERLAPPEDWINDOW,
  //   0, 0, formWidth, formHeight, NULL, NULL, GetModuleHandle(NULL), NULL
  // );
  // assert(h_window != NULL);

  // if (rhi == FORM_RHI_D3D11) {
  //   render = new RenderD3D11;
  //   dynamic_cast<RenderD3D11*>(render)->getHwnd(h_window);
  // }
  // else if (rhi == FORM_RHI_OPENGL) {
  //   render = new RenderGL;
  //   dynamic_cast<RenderGL*>(render)->getHwnd(h_window);
  // }
  // else {
  //   std::cout << "Form Init Failed: Illegal Render API" << std::endl;
  //   return;
  // }

  // render->Init();

  h_window = CreateWindowEx(0, title.c_str(), title.c_str(),
                            WS_OVERLAPPEDWINDOW,
                            0, 0, formWidth, formHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
  assert(h_window != NULL);

  ShowWindow(h_window, SW_SHOW);

  if (rhi == FORM_RHI_OPENGL) {
    m_hdc = GetDC(h_window);
    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW |
      PFD_SUPPORT_OPENGL |
      PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      24,
      0, 0, 0, 0, 0, 0,                 // color bits ignored  
      0,
      0,
      0,
      0, 0, 0, 0,                       // accum bits ignored  
      32,                               // 32-bit z-buffer      
      0,
      0,
      PFD_MAIN_PLANE,
      0,
      0, 0, 0                           // layer masks ignored  
    };
    int iPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    SetPixelFormat(m_hdc, iPixelFormat, &pfd);
    auto m_hrc = wglCreateContext(m_hdc);
    wglMakeCurrent(m_hdc, m_hrc);
  }
  else {
    std::cout << "Form Init Failed: Illegal Render API" << std::endl;
    return;
  }
  
  return;
}

void FormWin::DestroyForm() {

}

int FormWin::DisplayFrame(unsigned char* buffer) {
  if (!buffer) {
    if (GetMessage(&msg, NULL, 0, 0)) {
      if (msg.message == WM_QUIT) return 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    return 1;
  }
  return 0;
}

void FormWin::LoadScene(std::string_view scene_path) {

}

LRESULT CALLBACK FormWin::myWndProc(
  HWND hwnd,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
) {
  LRESULT hr = 0;
  switch (message)
  {
    case WM_PAINT:
    SwapBuffers(m_hdc);
    case WM_DESTROY:
    PostQuitMessage(0);
    break;
    case WM_DISPLAYCHANGE:
    InvalidateRect(hwnd, nullptr, false);
    default: return DefWindowProc(hwnd, message, wparam, lparam);
  }
  return hr;
}