#pragma once
#include "../Component.h"
#include "../Texture.h"
#include "../../Core/eigen-3.4.0/Eigen/Eigen"

class Material : public Component {
public:
  std::vector<Texture> diffuse;
  std::vector<Texture> normal;
  Eigen::Vector3f Ka, Kd, Ks;
  float_t Ns;
  float_t Ni;
  float_t d;
  uint32_t illum;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Material, diffuse, normal)
};