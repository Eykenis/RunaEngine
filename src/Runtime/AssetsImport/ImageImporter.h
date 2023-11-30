#pragma once
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "../../../exlib/libpng/include/png.h"

struct ImageBufferHeader {
  void* data;
  uint32_t width, height;
  uint32_t channel_depth;
  enum class ColorType {
    RGB,
    RGBA,
    Grayscale,
    GrayscaleA,
    Palette,
  };
  ColorType color_type;
};

class ImageImporter {
public:
  static ImageBufferHeader* ReadPNG(const std::string& fileName);
};