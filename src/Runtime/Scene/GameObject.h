#pragma once
#include "Components/Components.h"
#include "BaseObject.h"
#include "../Core/Memory/MemoryManager.h"

#include <crossguid/guid.hpp>

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <unordered_map>

#define MAX_COMPONENT_COUNT 16

class GameObject : public BaseObject {
private:
  std::string name; // name represented in editor
  GameObject* ParentGameObject;
  std::list<GameObject*> ChildGameObjects;
  std::unordered_map<std::string, std::list<Component*>> Componentss;
protected:
  void AppendChild(GameObject* subObject) {
    ChildGameObjects.push_back(subObject);
    subObject->ParentGameObject = this;
  }
public:
  GameObject(std::string_view names = "GameObject");
  ~GameObject() { }

  const xg::Guid& GetGUID() { return guid; }

  friend class SceneManager;

// templates

  // only works when using class inherited from Component
  template<typename T>
  std::enable_if_t<std::is_base_of<Component, T>::value, T*>
  AddComponent() {
    T* t = new T;
    // Components.emplace_back(t);
    Componentss[typeid(T).name()].emplace_back(t);
    return t;
  }

  template<typename T>
  std::enable_if_t<std::is_base_of<Component, T>::value, T*>
  AddComponent(T* t) {
    // Components.emplace_back(t);
    Componentss[typeid(T).name()].emplace_back(t);
    return t;
  }

  template<typename T>
  uint32_t GetComponentCount() {
    return Componentss[typeid(T).name()].size();
  }

  // get/remove components
  template<typename T>
  std::enable_if_t<std::is_base_of<Component, T>::value, Component*>
  GetComponent(uint32_t index = 0) {
    uint32_t cnt = 0;
    for (auto it = Componentss[typeid(T).name()].begin(); it != Componentss[typeid(T).name()].end(); ++it) {
      if (cnt == index) {
        return *it;
      }
      cnt++;
    }
    return nullptr;
  }

  template<typename T>
  std::enable_if_t<std::is_base_of<Component, T>::value, void>
  RemoveComponent(uint32_t index = 0) {
    uint32_t idx = 0;
    for (auto it = Componentss.begin(); it != Componentss.end(); ++it) {
      if (typeid(*it).name() == typeid(T).name()) {
        if (idx == index) {
          Componentss.erase(it);
          return;
        }
        idx++;
      }
    }
    return;
  }
};