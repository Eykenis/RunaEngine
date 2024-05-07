#pragma once

#include <vector>
#include <list>

#include "../Component.h"

class Mesh;

struct MeshNode {
  std::vector<Mesh> meshes;
  std::list<MeshNode*> children;
};

class MeshReference : public Component {
public:
  MeshNode* rootNode;
};