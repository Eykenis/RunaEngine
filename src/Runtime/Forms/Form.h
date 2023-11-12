#pragma once
#include <string>

class Form {
public:
  virtual void InitForm(int formWidth, int formHeight, std::string title) = 0;
  virtual void DestroyForm() = 0;
  virtual int DisplayFrame(unsigned char* buffer) = 0;
};