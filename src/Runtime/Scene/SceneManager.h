#pragma once
#include "GameObject.h"
#include "../RHI/GraphicsManagerModule.h"
#include "../RHI/OpenGL/GraphicsManagerGL.h"
#include "Components/ShaderReference.h"
#include "Components/Camera.h"
#include "../RHI/ShaderModule.h"
#include "LightSource.h"
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include "SerializeHelper.h"

class SceneManager {
private:
  GameObject* HierachyRoot;
  GameObject* MainCamera;
  std::vector<LightSource*> lightSource;
  void InitSingleMeshRenderable(std::shared_ptr<MeshNode> current, uint32_t shader_idx);
  void SeekEveryShaderReference(GameObject* current);
  void ClearSingleSceneRenderable(GameObject* current);
  void ReleaseSingleMeshRenderable(std::shared_ptr<MeshNode> current);
  std::string sceneName;
public:
  SceneManager(GameObject* root = new GameObject, uint32_t fW = 640, uint32_t fH = 480) : HierachyRoot(root) {
    graphics_manager = new GraphicsManagerGL;
    graphics_manager->Init(fW, fH);
  }
  void Destroy(GameObject* gameObjectToDestroy);
  void AddNewGameObject(GameObject* newGameObject, GameObject* parentGameObject = nullptr);
  void AddLightSource(LightSource* lightSource);
  ~SceneManager();
  void SaveScene(const std::string path);
  // reset all MVP matrix
  void ResetModelMatrix(GameObject* current);
  void ResetSingleMeshModelMatrix(std::shared_ptr<MeshNode> current, Eigen::Matrix4f model);
  void ResetModelMatrixRecur(GameObject* current);
  void InitSingleSceneRenderable(GameObject* current);
  void InitSceneRenderable();
  void ClearSceneRenderable();
  void RenderScene();
  void RenderScene(GameObject*);
  void loadScene(std::string_view path);
  // void loadSingleGameObject()
  void SetMainCamera(GameObject*);
  static SceneManager* ReadScene(std::string_view scene_path);
  void SaveGameObject(nlohmann::json &scenejson, GameObject* go, GameObject* parent);
  GraphicsManagerModule *graphics_manager;
  std::vector<ShaderReference> ShaderIndex;
};