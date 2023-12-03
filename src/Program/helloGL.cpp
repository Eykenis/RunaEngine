#include <cassert>
#include <cstdint>
#include <windows.h>

#include <glad/glad.h>
#include <wingdi.h>
#include <winuser.h>

const uint32_t SCREEN_WIDTH = 640;
const uint32_t SCREEN_HEIGHT = 480;

struct VERTEX {
  float x, y, z;
  float r, g, b;
};

// Context
uint32_t VBO, VAO;
uint32_t vertexShader;
uint32_t fragmentShader;
uint32_t shaderProgram;

// resources
VERTEX vertices[] = {
  {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
  {0.45f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
  {-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f},
};

const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(ourColor, 1.0);\n"
"}\0";

LRESULT CALLBACK myWndProc(
  HWND hwnd,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
);

bool InitOpenGL(HWND hwnd, HDC* hdc, HGLRC* hrc);
void DrawFrame(HDC* hdc);
void DeleteResource(HWND hwnd, HDC hdc, HGLRC hrc);

INT WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR pCmdLine,
  int nCmdShow
) {
  ATOM atom;
  WNDCLASSEX wc;
  MSG msg;
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.lpfnWndProc = (WNDPROC)myWndProc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpszClassName = "HelloGL";

  atom = RegisterClassEx(&wc);
  assert(atom != 0);

  HWND hwnd = CreateWindowEx(
    0,
    "HelloGL",
    "hello, openGL",
    WS_OVERLAPPEDWINDOW,
    0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL
  );
  assert(hwnd != NULL);
  ShowWindow(hwnd, nCmdShow);

  HDC hdc;
  HGLRC hrc;

  assert(InitOpenGL(hwnd, &hdc, &hrc));
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  DeleteResource(hwnd, hdc, hrc);

  DestroyWindow(hwnd);

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
    {
      HDC hdc = GetDC(hwnd);
      DrawFrame(&hdc);
    }
    break;
    case WM_DESTROY:
    PostQuitMessage(0);
    break;
    case WM_DISPLAYCHANGE:
    InvalidateRect(hwnd, nullptr, false);
    default: return DefWindowProc(hwnd, message, wparam, lparam);
  }
  return hr;
}

bool InitOpenGL(HWND hwnd, HDC* hdc, HGLRC* hrc) {
  // Make Context First
  *hdc = GetDC(hwnd);
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
  int iPixelFormat = ChoosePixelFormat(*hdc, &pfd);
  SetPixelFormat(*hdc, iPixelFormat, &pfd);
  *hrc = wglCreateContext(*hdc);
  wglMakeCurrent(*hdc, *hrc);

  if (!gladLoadGL()) {
    return false;
  }
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return true;
}

void DrawFrame(HDC* hdc) {
  glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  SwapBuffers(*hdc);
  Sleep(1);
}

void DeleteResource(HWND hwnd, HDC hdc, HGLRC hrc) {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // release context
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(hrc);
  ReleaseDC(hwnd, hdc);
}