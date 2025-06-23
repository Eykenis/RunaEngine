#include "MeshImporter.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/types.h"
#include <cassert>
#include <memory>

std::vector<Texture> LoadTexture(aiMaterial* mat, aiTextureType type) {
  std::vector<Texture> textures;
  for (uint32_t i = 0; i < mat->GetTextureCount(type); ++i) {
    aiString str;
    mat->GetTexture(type, i, &str);
    Texture tex;
    tex.path = str.C_Str();
    textures.emplace_back(tex);
  }
  return textures;
}

void LoadLightingParams(Material& targetMat, aiMaterial* mat) {
  aiColor3D tmpVec;
  mat->Get(AI_MATKEY_SHININESS, targetMat.Ns);
  mat->Get(AI_MATKEY_REFRACTI, targetMat.Ni);
  mat->Get(AI_MATKEY_OPACITY, targetMat.d);
  mat->Get(AI_MATKEY_SHADING_MODEL, targetMat.illum);

  if (mat->Get(AI_MATKEY_COLOR_AMBIENT, tmpVec) == AI_SUCCESS) {
    targetMat.Ka = Eigen::Vector3f(tmpVec.r, tmpVec.g, tmpVec.b);
  }
  else targetMat.Ka = Eigen::Vector3f(1.0f, 1.0f, 1.0f);

  if (mat->Get(AI_MATKEY_COLOR_SPECULAR, tmpVec) == AI_SUCCESS) {
    targetMat.Ks = Eigen::Vector3f(tmpVec.r, tmpVec.g, tmpVec.b);
  }
  else targetMat.Ks = Eigen::Vector3f(1.0f, 1.0f, 1.0f);

  if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, tmpVec) == AI_SUCCESS) {
    targetMat.Kd = Eigen::Vector3f(tmpVec.r, tmpVec.g, tmpVec.b);
  }
  else targetMat.Kd = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
}

Mesh MeshImporter::ProcessMesh(aiMesh* mesh, const aiScene *scene) {
  Mesh ret;
  // vertex
  for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
    ret.Position.emplace_back(mesh->mVertices[i].x);
    ret.Position.emplace_back(mesh->mVertices[i].y);
    ret.Position.emplace_back(mesh->mVertices[i].z);
    ret.Normal.emplace_back(mesh->mNormals[i].x);
    ret.Normal.emplace_back(mesh->mNormals[i].y);
    ret.Normal.emplace_back(mesh->mNormals[i].z);
  }
  for (uint32_t i = 0; i < MAX_TEXCOORD_COUNT; ++i) {
    if (!mesh->mTextureCoords[i]) break;
    ret.TexCoordCount++;
    // printf("TEXCOORD :%d\n", i);
    for (uint32_t j = 0; j < mesh->mNumVertices; ++j) {
      // printf("%f %f %f\n", mesh->mTextureCoords[i][j].x, mesh->mTextureCoords[i][j].y, mesh->mTextureCoords[i][j].z);
      ret.TexCoord[i].emplace_back(mesh->mTextureCoords[i][j].x);
      ret.TexCoord[i].emplace_back(mesh->mTextureCoords[i][j].y);
      ret.TexCoord[i].emplace_back(mesh->mTextureCoords[i][j].z);
    }
  }
  // index
  for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
    assert(mesh->mFaces[i].mNumIndices == 3);
    ret.Face.emplace_back(mesh->mFaces[i].mIndices[0]);
    ret.Face.emplace_back(mesh->mFaces[i].mIndices[1]);
    ret.Face.emplace_back(mesh->mFaces[i].mIndices[2]);
  }
  // material?
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    ret.material.diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
    ret.material.normal = LoadTexture(material, aiTextureType_NORMALS);
    LoadLightingParams(ret.material, material);
  }
  return ret;
}

void MeshImporter::ProcessNode(std::shared_ptr<MeshNode> meshNode, aiNode* node, const aiScene* scene) {
  for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshNode->meshes.emplace_back(MeshImporter::ProcessMesh(mesh, scene));
  }
  for (uint32_t i = 0; i < node->mNumChildren; ++i) {
    std::shared_ptr<MeshNode> child = std::make_shared<MeshNode>();
    meshNode->children.push_back(child);
    ProcessNode(child, node->mChildren[i], scene);
  }
}

MeshReference* MeshImporter::ReadMesh(const std::string& fileName) {
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << import.GetErrorString() << std::endl;
    return nullptr;
  }
  MeshReference* reference = new MeshReference;
  reference->filepath = fileName;
  reference->rootNode = std::make_shared<MeshNode>();
  ProcessNode(reference->rootNode, scene->mRootNode, scene);
  return reference;
}