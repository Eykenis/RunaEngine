#include "SceneManager.h"
#include "Components/MeshReference.h"
#include "GameObject.h"

void SceneManager::Destroy(GameObject* gameObjectToDestroy) {
  GameObject* ParentGO = gameObjectToDestroy->ParentGameObject;
  for (auto it = ParentGO->ChildGameObjects.begin(); it != ParentGO->ChildGameObjects.end(); ++it) {
    if ((*it)->GetGUID() == gameObjectToDestroy->GetGUID()) {
      ParentGO->ChildGameObjects.erase(it);
      break;
    }
  }
  for (auto& x : gameObjectToDestroy->ChildGameObjects) {
    // recursive
    Destroy(x);
  }
  // release memory
  MemoryManager::GetInstance()->Delete(&gameObjectToDestroy);
}

SceneManager::~SceneManager() {
  Destroy(HierachyRoot);
}

void SceneManager::InitSingleMeshRenderable(MeshNode* current) {
  int sz = current->meshes.size();
  for (int i = 0; i < sz; ++i) {
    graphics_manager->CreateRenderable(&current->meshes[i]);
  }
  for (auto x : current->children) {
    InitSingleMeshRenderable(x);
  }
}

void SceneManager::InitSingleSceneRenderable(GameObject* current) {
  for (auto x : current->ChildGameObjects) {
    InitSingleSceneRenderable(x);
  }
  uint32_t size = current->GetComponentCount<MeshReference>();
  if (size) {
    for (int j = 0; j < size; ++j) {
      auto c = dynamic_cast<MeshReference*>(current->GetComponent<MeshReference>(j));
      InitSingleMeshRenderable(c->rootNode);
    }
  }
}

void SceneManager::InitSceneRenderable() {
  InitSingleSceneRenderable(HierachyRoot);
}

void SceneManager::RenderScene() {
  graphics_manager->Clear();
  graphics_manager->DrawAll();
}

/*
  .runascene format:
  Element:
  {
    GameObjectTypeName
    GameObjectID
    FatherGameObjectID
    GameObjectDataLength
    GameObjectData
  }
*/
void SceneManager::SaveScene() {

}