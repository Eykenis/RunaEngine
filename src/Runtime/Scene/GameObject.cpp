#include "GameObject.h"

GameObject::GameObject(std::string_view names) {
  name = names;
  guid = xg::newGuid();
  AddComponent<Transform>();
}