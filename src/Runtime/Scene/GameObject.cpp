#include "GameObject.h"

GameObject::GameObject() {
  guid = xg::newGuid();
  AddComponent<Transform>();
}