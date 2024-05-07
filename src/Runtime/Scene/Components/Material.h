#pragma once
#include "../Component.h"
#include "../Texture.h"

class Material : public Component {
public:
  std::vector<Texture> diffuse;
  std::vector<Texture> normal;
};