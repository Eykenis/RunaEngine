#pragma once
#include "GameObject.h"
#include "../RHI/GraphicsManagerModule.h"
#include "../RHI/OpenGL/GraphicsManagerGL.h"

class SceneManager {
private:
  GameObject* HierachyRoot;
  void InitSingleSceneRenderable(GameObject* current);
  void InitSingleMeshRenderable(MeshNode* current);
public:
  SceneManager(GameObject* root = new GameObject, uint32_t fH = 640, uint32_t fW = 480) : HierachyRoot(root) {
    graphics_manager = new GraphicsManagerGL;
    graphics_manager->Init(fH, fW);
  }
  void Destroy(GameObject* gameObjectToDestroy);
  ~SceneManager();
  void SaveScene();
  void InitSceneRenderable();
  void RenderScene();
  static SceneManager* ReadScene(std::string_view scene_path);
  GraphicsManagerModule *graphics_manager;
};