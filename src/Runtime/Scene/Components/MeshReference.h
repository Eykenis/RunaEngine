#pragma once

#include <vector>
#include <list>

#include "../Component.h"
#include "../Mesh.h"

// #define NLOHMANN_DEFINE_TYPE_INTRUSIVE_SHARED_PTR(Type, ...)                  \
//   friend void to_json(nlohmann::json& j, const Type& obj) {                  \
//     nlohmann::json temp_json;                                                \
//     nlohmann::to_json(temp_json, obj);                                       \
//     j = temp_json;                                                           \
//   }                                                                           \
//   friend void from_json(const nlohmann::json& j, Type& obj) {                \
//     nlohmann::from_json(j, obj);                                             \
//   }

class MeshNode {
public:
  std::vector<Mesh> meshes;
  std::vector<int> renderableIdx; // for picking renderable
  std::list<std::shared_ptr<MeshNode>> children;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(MeshNode, meshes, renderableIdx, children)
};

class MeshReference : public Component {
public:
  std::shared_ptr<MeshNode> rootNode;
  std::string filepath;
  // TODO: only save path in serialization. Do assetimport in deserialization
  // therefore from_json() should be rewrite
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(MeshReference, filepath)
};