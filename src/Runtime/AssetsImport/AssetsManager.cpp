#include "AssetsManager.h"

#define ASSET_ERRFORMAT  0
#define ASSET_PNG        1

AssetsManager::AssetsManager() {
  
}

AssetsManager::~AssetsManager() {
  
}

void* AssetsManager::LoadAsset(const std::string& fileName) {
  switch(FindAssetType(fileName)) {
    case ASSET_PNG:
      return ImageImporter::ReadPNG(fileName);
    case ASSET_ERRFORMAT:
      return nullptr;
    default:
      return nullptr;
  }
}

int AssetsManager::FindAssetType(const std::string& fileName) {
  std::string subfix;
  for (uint32_t i = fileName.size() - 1; i >= 0; --i) {
    if (fileName[i] == '.') break;
    subfix += fileName[i];
  }
  std::reverse(subfix.begin(), subfix.end());
  
  if (subfix == "PNG" || subfix == "png") return ASSET_PNG;
  return ASSET_ERRFORMAT;
}