#include "MeshImporter.h"

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

Mesh MeshImporter::ProcessMesh(aiMesh* mesh, const aiScene *scene) {
  Mesh ret;
  // MVP matrix'll be add in GO

  // vertex
  for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
    ret.Position.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    ret.Normal.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
  }
  for (uint32_t i = 0; i < MAX_TEXCOORD_COUNT; ++i) {
    if (!mesh->mTextureCoords[i]) break;
    ret.TexCoordCount++;
    for (uint32_t j = 0; j < mesh->mNumVertices; ++j) {
      ret.TexCoord[i].emplace_back(mesh->mTextureCoords[i][j].x, mesh->mTextureCoords[i][j].y);
    }
  }
  // index
  for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
    assert(mesh->mFaces[i].mNumIndices == 3);
    ret.Face.emplace_back(mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]);
  }
  // material?
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    ret.material.diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
    ret.material.normal = LoadTexture(material, aiTextureType_NORMALS);
  }
  return ret;
}

void MeshImporter::ProcessNode(MeshNode* meshNode, aiNode* node, const aiScene* scene) {
  for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshNode->meshes.emplace_back(MeshImporter::ProcessMesh(mesh, scene));
  }
  for (uint32_t i = 0; i < node->mNumChildren; ++i) {
    MeshNode* child = new MeshNode;
    meshNode->children.push_back(child);
    ProcessNode(child, node->mChildren[i], scene);
  }
}

MeshReference* MeshImporter::ReadMesh(const std::string& fileName) {
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile(fileName, aiProcess_Triangulate);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << import.GetErrorString() << std::endl;
    return nullptr;
  }
  MeshReference* reference = new MeshReference;
  reference->rootNode = new MeshNode;
  ProcessNode(reference->rootNode, scene->mRootNode, scene);
  return reference;
}