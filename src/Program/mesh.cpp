#include "../Runtime/Core/eigen-3.4.0/Eigen/Eigen"
#include "../Runtime/Scene/SceneManager.h"
#include "../Runtime/AssetsImport/MeshImporter.h"
#include <iostream>

void check(MeshNode* node, int rnk) {
  std::cout << "mesh node rank " << rnk << std::endl;
  std::cout << "mesh count: " << node->meshes.size() << std::endl;
  for (uint32_t i = 0; i < node->meshes.size(); ++i) {
    std::cout <<  "face count of the " << i + 1 << "th mesh: "<< node->meshes[i].Face.size() << std::endl;
  }
  for (auto &x : node->children) {
    check(x, rnk + 1);
  }
}

int main()
{
  GameObject* RootObject = new GameObject;
  SceneManager* scene = new SceneManager(RootObject);
  MeshReference* thisMesh = RootObject->AddComponent<MeshReference>(MeshImporter::ReadMesh("../assets/models/Aiz.obj"));
  MeshReference* getThisMesh = dynamic_cast<MeshReference*>(RootObject->GetComponent<MeshReference>());
  // std::cout << "Face Count of the First Mesh: " << getThisMesh->rootNode->meshes[0].Face.size() << std::endl;
  check(getThisMesh->rootNode, 0);
  return 0;
}