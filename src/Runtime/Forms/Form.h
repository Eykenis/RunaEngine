#pragma once
#include <string>
#define FORM_RHI_OPENGL 1
#define FORM_RHI_D3D11 2

class Form {
public:
  virtual void InitForm(int formWidth, int formHeight, std::string title, int rhi = 1) = 0;
  virtual void DestroyForm() = 0;
  virtual int DisplayFrame(unsigned char* buffer) = 0;
};