#pragma once
#include "GameObject.h"
#include "../RHI/GraphicsManagerModule.h"
#include "../RHI/OpenGL/GraphicsManagerGL.h"
#include "Components/ShaderReference.h"
#include "Camera.h"
#include "../RHI/ShaderModule.h"
#include <cstdint>

class SceneManager {
private:
  GameObject* HierachyRoot;
  Camera* MainCamera;
  void InitSingleMeshRenderable(MeshNode* current, uint32_t shader_idx);
  void SeekEveryShaderReference(GameObject* current);
  void ClearSingleSceneRenderable(GameObject* current);
  void ReleaseSingleMeshRenderable(MeshNode* current);
public:
  SceneManager(GameObject* root = new GameObject, uint32_t fW = 640, uint32_t fH = 480) : HierachyRoot(root) {
    graphics_manager = new GraphicsManagerGL;
    graphics_manager->Init(fW, fH);
  }
  void Destroy(GameObject* gameObjectToDestroy);
  void AddNewGameObject(GameObject* newGameObject, GameObject* parentGameObject = nullptr);
  ~SceneManager();
  void SaveScene();
  // reset all MVP matrix
  void ResetModelMatrix(GameObject* current);
  void ResetSingleMeshModelMatrix(MeshNode* current, Eigen::Matrix4f model);
  void ResetModelMatrixRecur(GameObject* current);
  void InitSingleSceneRenderable(GameObject* current);
  void InitSceneRenderable();
  void ClearSceneRenderable();
  void RenderScene();
  void RenderScene(Camera*);
  void SetMainCamera(Camera*);
  static SceneManager* ReadScene(std::string_view scene_path);
  GraphicsManagerModule *graphics_manager;
  std::vector<ShaderReference> ShaderIndex;
};