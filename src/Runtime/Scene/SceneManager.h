#pragma once
#include "GameObject.h"

class SceneManager {
private:
  GameObject* HierachyRoot;
public:
  SceneManager(GameObject* root = new GameObject) : HierachyRoot(root) { };
  void Destroy(GameObject* gameObjectToDestroy);
  ~SceneManager();
};