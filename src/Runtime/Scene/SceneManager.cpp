#include "SceneManager.h"

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