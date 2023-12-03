#pragma once
#include <cassert>
#include <windows.h>
#include <tchar.h>
#include <wingdi.h>
#include <winuser.h>

#include "../Form.h"

class FormWin : public Form {
public:
  void InitForm(int formWidth, int formHeight, std::string title, int rhi) override;
  void DestroyForm() override;
  int DisplayFrame(unsigned char* buffer = 0) override;
private:
  WNDCLASSEX wc;
  HWND h_window;
  MSG msg;
};