#pragma once
#include <cassert>
#include <windows.h>
#include <tchar.h>
#include <wingdi.h>
#include <winuser.h>
#include <dxgi.h>

#include <iostream>

#include "../Form.h"

class FormWin : public Form {
public:
  void InitForm(int formWidth, int formHeight, std::string title, int rhi) override;
  void DestroyForm() override;
  void LoadScene(std::string_view scene_path) override;
  int DisplayFrame(unsigned char* buffer = 0) override;
  void ClearFrame() override;
  HWND getHwnd();
private:
  uint16_t rhi_type;
  WNDCLASSEX wc;
  HWND h_window;
  MSG msg;
  static LRESULT CALLBACK myWndProc(
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam
  );
};