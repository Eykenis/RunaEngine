#pragma once
#include "BaseObject.h"
// #include "../Core/eigen-3.4.0/Eigen/Eigen"
#include "Components/Material.h"

#define MAX_TEXCOORD_COUNT 8

class Mesh : BaseObject {
public:
  uint32_t TexCoordCount;
  std::vector<float> Position;
  std::vector<float> Normal;
  std::vector<int> Face; // only triangle supported
  std::array<std::vector<float>, MAX_TEXCOORD_COUNT> TexCoord;
  Material material;
  
  Mesh() : TexCoordCount(0) { }
  virtual ~Mesh() { }
  friend class MeshImporter;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Mesh, TexCoordCount, Position, Normal, Face, TexCoord, material)
};