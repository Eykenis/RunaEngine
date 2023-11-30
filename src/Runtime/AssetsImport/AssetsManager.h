#pragma once
#include <string>
#include "ImageImporter.h"

class AssetsManager {
private:
public:
  AssetsManager();
  virtual ~AssetsManager();
  void* LoadAsset(const std::string& fileName);
  int FindAssetType(const std::string& fileName);
};