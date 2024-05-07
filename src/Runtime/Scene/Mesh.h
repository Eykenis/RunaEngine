#pragma once
#include "BaseObject.h"
#include "../Core/eigen-3.4.0/Eigen/Eigen"
#include "Components/Material.h"

#define MAX_TEXCOORD_COUNT 8

class Mesh : BaseObject {
public:
  uint32_t TexCoordCount;
  std::vector<Eigen::Vector3f> Position;
  std::vector<Eigen::Vector2f> TexCoord[MAX_TEXCOORD_COUNT];
  std::vector<Eigen::Vector3f> Normal;
  std::vector<Eigen::Vector3i> Face; // only triangle supported
  Material material;
  
  Mesh() : TexCoordCount(0) { };
  virtual ~Mesh() { };
  friend class MeshImporter;
};