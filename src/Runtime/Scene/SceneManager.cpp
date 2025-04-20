#include "SceneManager.h"
#include "Components/MeshReference.h"
#include "GameObject.h"
#include "Components/ShaderReference.h"
// #include "src/Runtime/Core/eigen-3.4.0/Eigen/src/Geometry/Rotation2D.h"
#include "../Core/eigen-3.4.0/Eigen/src/Geometry/Transform.h"
// #include "src/Runtime/Scene/Component.h"
#include "../Scene/Components/Transform.h"
#include "../Core/eigen-3.4.0/Eigen/src/Geometry/AngleAxis.h"
#include "../Scene/PointLightSource.h"
#include <cstdint>

void SceneManager::Destroy(GameObject* gameObjectToDestroy) {
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

void SceneManager::ReleaseSingleMeshRenderable(MeshNode* current) {
  int sz = current->renderableIdx.size();
  for (int i = 0 ; i < sz; ++i) {
    graphics_manager->ReleaseRenderable(current->renderableIdx[i]);
  }
  for (auto x : current->children) {
    ReleaseSingleMeshRenderable(x);
  }
}

void SceneManager::SetMainCamera(Camera* cam) {
  this->MainCamera = cam;
}

SceneManager::~SceneManager() {
  Destroy(HierachyRoot);
}

void SceneManager::InitSingleMeshRenderable(MeshNode* current, uint32_t shader_idx) {
  int sz = current->meshes.size();
  current->renderableIdx.clear();
  for (int i = 0; i < sz; ++i) {
    current->renderableIdx.emplace_back(graphics_manager->CreateRenderable(&current->meshes[i], shader_idx));
  }
  for (auto x : current->children) {
    InitSingleMeshRenderable(x, shader_idx);
  }
}

void SceneManager::ResetSingleMeshModelMatrix(MeshNode* current, Eigen::Matrix4f model) {
  int sz = current->meshes.size();
  for (int i = 0; i < sz; ++i) {
    graphics_manager->setMat4(current->renderableIdx[i], model);
  }
  for (auto x : current->children) {
    ResetSingleMeshModelMatrix(x, model);
  }
}

void SceneManager::ResetModelMatrix(GameObject* current) {
  Eigen::Matrix4f perspective;
  perspective <<  1 / (tan(MainCamera->fovY / 2) * MainCamera->aspect), 0, 0, 0,
                  0, 1 / tan(MainCamera->fovY / 2), 0, 0,
                  0, 0, (MainCamera->zFar + MainCamera->zNear) / (MainCamera->zFar - MainCamera->zNear), -2 * MainCamera->zFar * MainCamera->zNear / (MainCamera->zFar - MainCamera->zNear),
                  0, 0, 1, 0;

  auto camR = MainCamera->camDirection.cross(MainCamera->camUp);
  auto tf = dynamic_cast<Transform*>(MainCamera->GetComponent<Transform>());
  
  Eigen::Matrix4f lookAt;
  Eigen::Matrix4f tr;

  lookAt << camR.x(), camR.y(), camR.z(), 0,
            MainCamera->camUp.x(), MainCamera->camUp.y(), MainCamera->camUp.z(), 0,
            MainCamera->camDirection.x(), MainCamera->camDirection.y(), MainCamera->camDirection.z(), 0,
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
  for (auto x : current->ChildGameObjects) {
    ResetModelMatrixRecur(x);
  }
  uint32_t size = current->GetComponentCount<MeshReference>();
  if (size) {
    for (int j = 0; j < size; ++j) {
      uint32_t size = current->GetComponentCount<MeshReference>();
      auto tf = dynamic_cast<Transform*>(current->GetComponent<Transform>());
      Eigen::Matrix4f model;

      Eigen::Affine3f transform = Eigen::Affine3f::Identity();
      transform.scale(tf->scale());
      transform.rotate(
        Eigen::AngleAxisf(tf->rotation().x(), Eigen::Vector3f::UnitX()) * 
        Eigen::AngleAxisf(tf->rotation().y(), Eigen::Vector3f::UnitY()) * 
        Eigen::AngleAxisf(tf->rotation().z(), Eigen::Vector3f::UnitZ())
        );
      transform.translate(tf->position());

      model = transform.matrix();

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