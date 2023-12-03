#pragma once

#include "../RenderModule.h"
#include "../../Core/eigen-3.4.0/Eigen/Eigen"
#include <glad/glad.h>

#include <windows.h>
#include <tchar.h>

class RenderGL : public RenderModule {
public:
  virtual int init();
  virtual int release();
  virtual void clear();
  virtual void draw();
  virtual ~RenderGL();

  void getHwnd(HWND hwnd);
private:
  HWND m_hwnd;
  HDC m_hdc;
  HGLRC m_hrc;
};