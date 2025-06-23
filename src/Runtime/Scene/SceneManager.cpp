#include "SceneManager.h"
#include "Components/MeshReference.h"
#include "GameObject.h"
#include "Components/ShaderReference.h"
// #include "src/Runtime/Core/eigen-3.4.0/Eigen/src/Geometry/Rotation2D.h"
// #include "src/Runtime/Scene/Component.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/PointLightSource.h"
#include "nlohmann/json.hpp"
#include <cstdint>
#include <exception>
#include <memory>

void SceneManager::Destroy(GameObject* gameObjectToDestroy) {
  if (gameObjectToDestroy == nullptr) return;
  GameObject* ParentGO = gameObjectToDestroy->ParentGameObject;
  if (gameObjectToDestroy->ParentGameObject) {
    for (auto it = ParentGO->ChildGameObjects.begin(); it != ParentGO->ChildGameObjects.end(); ++it) {
      if ((*it)->GetGUID() == gameObjectToDestroy->GetGUID()) {
        ParentGO->ChildGameObjects.erase(it);
        break;
      }
    }
  }
  for (auto& x : gameObjectToDestroy->ChildGameObjects) {
    // recursive
    Destroy(x);
  }
  // release renderable
  uint32_t size = gameObjectToDestroy->GetComponentCount<MeshReference>();
  if (size) {
    for (int j = 0; j < size; ++j) {
      auto c = dynamic_cast<MeshReference*>(gameObjectToDestroy->GetComponent<MeshReference>(j));
      ReleaseSingleMeshRenderable(c->rootNode);
    }
  }
  // release memory
  MemoryManager::GetInstance()->Delete(&gameObjectToDestroy);
  // delete gameObjectToDestroy;
}

void SceneManager::ReleaseSingleMeshRenderable(std::shared_ptr<MeshNode> current) {
  int sz = current->renderableIdx.size();
  for (int i = 0 ; i < sz; ++i) {
    graphics_manager->ReleaseRenderable(current->renderableIdx[i]);
  }
  for (auto x : current->children) {
    ReleaseSingleMeshRenderable(x);
  }
}

void SceneManager::SetMainCamera(GameObject* cam) {
  this->MainCamera = cam;
}

SceneManager::~SceneManager() {
  Destroy(HierachyRoot);
}

void SceneManager::InitSingleMeshRenderable(std::shared_ptr<MeshNode> current, uint32_t shader_idx) {
  int sz = current->meshes.size();
  current->renderableIdx.clear();
  for (int i = 0; i < sz; ++i) {
    current->renderableIdx.emplace_back(graphics_manager->CreateRenderable(&current->meshes[i], shader_idx));
  }
  for (auto x : current->children) {
    InitSingleMeshRenderable(x, shader_idx);
  }
}

void SceneManager::ResetSingleMeshModelMatrix(std::shared_ptr<MeshNode> current, Eigen::Matrix4f model) {
  int sz = current->meshes.size();
  for (int i = 0; i < sz; ++i) {
    graphics_manager->setModel(current->renderableIdx[i], model);
  }
  for (auto x : current->children) {
    ResetSingleMeshModelMatrix(x, model);
  }
}

void SceneManager::ResetModelMatrix(GameObject* current) {
  Eigen::Matrix4f perspective;
  Camera* MCam = (Camera*)MainCamera->GetComponent<Camera>();
  perspective <<  1 / (tan(MCam->fovY / 2) * MCam->aspect), 0, 0, 0,
                  0, 1 / tan(MCam->fovY / 2), 0, 0,
                  0, 0, (MCam->zFar + MCam->zNear) / (MCam->zFar - MCam->zNear), -2 * MCam->zFar * MCam->zNear / (MCam->zFar - MCam->zNear),
                  0, 0, 1, 0;

  auto camR = MCam->camDirection.cross(MCam->camUp);
  auto tf = dynamic_cast<Transform*>(MainCamera->GetComponent<Transform>());
  
  Eigen::Matrix4f lookAt;
  Eigen::Matrix4f tr;

  lookAt << camR.x(), camR.y(), camR.z(), 0,
            MCam->camUp.x(), MCam->camUp.y(), MCam->camUp.z(), 0,
            MCam->camDirection.x(), MCam->camDirection.y(), MCam->camDirection.z(), 0,
            0, 0, 0, 1;
  
  tr << 1, 0, 0, -tf->position().x(),
        0, 1, 0, -tf->position().y(),
        0, 0, 1, -tf->position().z(),
        0, 0, 0, 1;

  lookAt = lookAt * tr;
  ResetModelMatrixRecur(current);
  graphics_manager->setView(lookAt);
  graphics_manager->setPerspective(perspective);
}

void SceneManager::ResetModelMatrixRecur(GameObject* current) {
  graphics_manager->setGlobalVec3("lightPos", ((PointLightSource*)lightSource[0])->lightPosition);
  graphics_manager->setGlobalVec3("camPos", ((Transform*)MainCamera->GetComponent<Transform>())->position());
  // graphics_manager->setGlobalFloat("metallic", 0.4);
  // graphics_manager->setGlobalFloat("roughness", 0.5);
  for (auto x : current->ChildGameObjects) {
    ResetModelMatrixRecur(x);
  }
  uint32_t size = current->GetComponentCount<MeshReference>();
  if (size) {
    for (int j = 0; j < size; ++j) {
      uint32_t size = current->GetComponentCount<MeshReference>();
      auto tf = dynamic_cast<Transform*>(current->GetComponent<Transform>());
      auto model = tf->getModelMatrix();

      if (size) {
        for (int j = 0; j < size; ++j) {
          auto c = dynamic_cast<MeshReference*>(current->GetComponent<MeshReference>(j));
          ResetSingleMeshModelMatrix(c->rootNode, model);
        }
      }
    }
  }
}

void SceneManager::SeekEveryShaderReference(GameObject* current) {
  for (auto x : current->ChildGameObjects) {
    SeekEveryShaderReference(x);
  }
  uint32_t size = current->GetComponentCount<MeshReference>();
  if (size) {
    for (int j = 0; j < size; ++j) {
      ShaderReference ref = *(dynamic_cast<ShaderReference*>(( current->GetComponent<ShaderReference>() )));
      bool count = 0;
      for (auto& x : ShaderIndex) {
        if (ref.isSame(x)) {
          count = 1;
          break;
        }
      }
      if (!count) {
        graphics_manager->CreateShader(ref.vpath.c_str(), ref.fpath.c_str());
        ShaderIndex.push_back(ref);
      }
    }
  }
}

void SceneManager::ClearSingleSceneRenderable(GameObject* current) {
  graphics_manager->ReleaseAll();
}

void SceneManager::InitSingleSceneRenderable(GameObject* current) {
  for (auto x : current->ChildGameObjects) {
    InitSingleSceneRenderable(x);
  }
  uint32_t size = current->GetComponentCount<MeshReference>();
  if (size) {
    for (int j = 0; j < size; ++j) {
      auto c = dynamic_cast<MeshReference*>(current->GetComponent<MeshReference>(j));
      ShaderReference ref = *(dynamic_cast<ShaderReference*>(( current->GetComponent<ShaderReference>() )));
      // find shader for it
      auto sz = ShaderIndex.size();
      for (int k = 0; k < sz; ++k) {
        if (ref.isSame(ShaderIndex[k])) {
          InitSingleMeshRenderable(c->rootNode, k);
          break;
        }
      }
    }
  }
}

void SceneManager::ClearSceneRenderable() {
  ClearSingleSceneRenderable(HierachyRoot);
}

void SceneManager::InitSceneRenderable() {
  SeekEveryShaderReference(HierachyRoot);
  InitSingleSceneRenderable(HierachyRoot);
}

void SceneManager::AddNewGameObject(GameObject* newGameObject, GameObject* parentGameObject) {
  if (parentGameObject == nullptr) parentGameObject = HierachyRoot;
  parentGameObject->ChildGameObjects.push_back(newGameObject);
  newGameObject->ParentGameObject = parentGameObject;
}

void SceneManager::AddLightSource(LightSource* lightSource) {
  this->lightSource.push_back(lightSource);
}

void SceneManager::RenderScene() {
  graphics_manager->Clear();
  ResetModelMatrix(HierachyRoot);
  graphics_manager->DrawAll();
}

void SceneManager::SaveScene(const std::string path) {
  nlohmann::json scenejson;
  scenejson["sceneName"] = this->sceneName;
  scenejson["mainCamera"] = this->MainCamera->GetGUID().str();
  SaveGameObject(scenejson, HierachyRoot, nullptr);
  std::ofstream outfile(path.c_str());
  outfile << scenejson.dump(2);
  outfile.close();
}

void SceneManager::SaveGameObject(nlohmann::json &scenejson, GameObject* go, GameObject* parent) {
  nlohmann::json subjson;
  if (parent)
    subjson["parent"] = parent->GetGUID().str();
  else
    subjson["parent"] = "null";
  subjson["name"] = go->name;
  for (auto &x : go->Componentss) {
    nlohmann::json componentjson;
    for (auto &y : x.second) {
      componentjson[y->guid] = ComponentSerializeHelper::getSerialized(typeid(*y), y);
    }
    subjson[x.first] = componentjson;
  }
  scenejson[go->GetGUID().str()] = subjson;
  for (auto &x : go->ChildGameObjects) {
    SaveGameObject(scenejson, x, go);
  }
}

void SceneManager::loadScene(std::string_view path) {
  // this->ClearSceneRenderable();
  // this->graphics_manager->Clear();
  // this->Destroy(HierachyRoot);

  std::ifstream file(std::string(path).c_str());
  if (!file.is_open()) {
    std::cerr << "Opening runascene file failed." << std::endl;
    return;
  }
  nlohmann::json j;
  try {
    file >> j;
  } catch (const std::exception& e) {
    std::cerr << "Failed to resolve runascene(json): " << e.what() << std::endl;
    return;
  }

}