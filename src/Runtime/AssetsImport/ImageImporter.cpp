#include "ImageImporter.h"

ImageBufferHeader* ImageImporter::ReadPNG(const std::string& fileName) {
  std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary);
  file.seekg(0);

  // check PNG header
  png_byte header[8];
  file.read(reinterpret_cast<char*>(header), 8);
  if (!file.good() || png_sig_cmp(header, 0, 8) != 0) {
    std::cerr << "Error: Not a PNG file" << std::endl;
    return nullptr;
  }

  // initialize necessary pointers of a single png image
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    return nullptr;
  }
  png_infop   info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return nullptr;
  }

  png_set_read_fn(png_ptr, &file, [](png_structp png_ptr, png_bytep png_data, png_size_t data_size) {
    std::istream& is = *static_cast<std::istream*>(png_get_io_ptr(png_ptr));
    is.read(reinterpret_cast<char*>(png_data), data_size);
  });

  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  // IHDR infos
  png_uint_32 img_width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 img_height = png_get_image_height(png_ptr, info_ptr);
  png_uint_32 bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  png_uint_32 img_channels = png_get_channels(png_ptr, info_ptr);
  png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png_ptr);
    bit_depth = 8;
  }
  else if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
    img_channels = 3;
  }

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
    img_channels++;
  }

  png_read_update_info(png_ptr, info_ptr);

  // real image data
  png_bytep *row_ptrs = new png_bytep[img_width];
  unsigned char *png_data = new unsigned char[img_width * img_height * img_channels * (bit_depth >> 3)];
  
  const uint32_t stride = img_width * img_channels;

  for (uint32_t i = 0; i < img_height; ++i) {
    png_uint_32 offset = (img_height - i - 1) * stride;
    row_ptrs[i] = reinterpret_cast<png_bytep>(png_data + offset);
  }

  png_read_image(png_ptr, row_ptrs);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  delete[] row_ptrs;

  ImageBufferHeader* ret = new ImageBufferHeader;
  ret->channel_depth = bit_depth;
  ret->data = png_data;
  ret->height = img_height;
  ret->width = img_width;
  if (color_type == PNG_COLOR_TYPE_GRAY) ret->color_type = ImageBufferHeader::ColorType::Grayscale;
  else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ret->color_type = ImageBufferHeader::ColorType::GrayscaleA;
  else if (color_type == PNG_COLOR_TYPE_RGB) ret->color_type = ImageBufferHeader::ColorType::RGB;
  else if (color_type == PNG_COLOR_TYPE_RGBA) ret->color_type = ImageBufferHeader::ColorType::RGBA;
  else if (color_type == PNG_COLOR_TYPE_PALETTE) ret->color_type = ImageBufferHeader::ColorType::Palette;
  
  return ret;
}