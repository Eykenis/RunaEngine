#pragma once
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include <png.h>

#include "../Scene/Components/MeshReference.h"
#include "../Scene/Components/Material.h"
#include "../Scene/Mesh.h"

class MeshImporter {
private:
  static void ProcessNode(MeshNode* meshNode, aiNode* node, const aiScene* scene);
  static Mesh ProcessMesh(aiMesh* mesh, const aiScene *scene);
public:
  static MeshReference* ReadMesh(const std::string& fileName);
};