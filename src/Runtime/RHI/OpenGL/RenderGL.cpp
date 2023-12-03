#include "RenderGL.h"

const uint32_t SCREEN_WIDTH = 640;
const uint32_t SCREEN_HEIGHT = 480;

struct VERTEX {
  float x, y, z;
  float r, g, b;
};

uint32_t VBO, VAO;
uint32_t vertexShader;
uint32_t fragmentShader;
uint32_t shaderProgram;

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

RenderGL::~RenderGL() {
  
}

void RenderGL::getHwnd(HWND hwnd) {
  m_hwnd = hwnd;
}

int RenderGL::init() {
  m_hdc = GetDC(m_hwnd);
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
  m_hrc = wglCreateContext(m_hdc);
  wglMakeCurrent(m_hdc, m_hrc);

  if (!gladLoadGL()) {
    return 0;
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

  return 1;
}

void RenderGL::clear() {
  glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void RenderGL::draw() {
  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  SwapBuffers(m_hdc);
}

int RenderGL::release() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // release context
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(m_hrc);
  ReleaseDC(m_hwnd, m_hdc);
}