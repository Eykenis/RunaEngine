#pragma once
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <png.h>

struct ImageBufferHeader {
  void* data;
  uint32_t width, height;
  uint32_t channel_depth;
  enum class ColorType {
    RGB = 3,
    RGBA = 4,
    Grayscale = 1,
    GrayscaleA = 2,
    Palette = 3,
  };
  ColorType color_type;
};

class ImageImporter {
public:
  static ImageBufferHeader* ReadPNG(const std::string& fileName);
};