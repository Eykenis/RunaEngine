#include "FormWin.h"
#include "../../RHI/D3D11/RenderD3D11.h"
#include "../../RHI/OpenGL/RenderGL.h"
#include "../../RHI/OpenGL/GraphicsManagerGL.h"
#include "../imgui/backends/imgui_impl_win32.h"
#include "../Form.h"
#include <cstddef>
#include <libloaderapi.h>
#include <wingdi.h>
#include <winuser.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HDC m_hdc;

LRESULT CALLBACK myWndProc(
  HWND hwnd,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
);

HWND FormWin::getHwnd() {
  return h_window;
}

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

  rhi_type = rhi;

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

  ShowWindow(h_window, SW_SHOW);
  UpdateWindow(h_window);
  
  return;
}

void FormWin::DestroyForm() {

}

void FormWin::ClearFrame() {
  if (rhi_type == FORM_RHI_OPENGL) {
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); 
  }
}

int FormWin::DisplayFrame(unsigned char* buffer) {
  if (!buffer) {
    if (GetMessage(&msg, NULL, 0, 0)) {
      // transport to ImGui
      if (ImGui_ImplWin32_WndProcHandler(h_window, msg.message, msg.wParam, msg.lParam)) {

      }
      if (msg.message == WM_QUIT) return 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return 1;
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