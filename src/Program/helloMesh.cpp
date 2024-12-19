#include "../Runtime/Core/eigen-3.4.0/Eigen/Eigen"
#include "../Runtime/Core/Memory/MemoryManager.h"
#include "../Runtime/Forms/Win32/FormWin.h"
#include "../Runtime/Scene/GameObject.h"
#include "../Runtime/Scene/SceneManager.h"
#include "../Runtime/Scene/Mesh.h"
#include "src/Runtime/AssetsImport/MeshImporter.h"
#include "src/Runtime/Scene/Components/MeshReference.h"
#include <iostream>

uint32_t fH = 720;
uint32_t fW = 720;
int main()
{
  // freopen("out.txt", "w", stdout);
    Form* form = dynamic_cast<FormWin*>(MemoryManager::GetInstance()->New<FormWin>());
    std::cout << "Select API you want. 1 for OpenGL, 2 for DirectX 11." << std::endl;
    int api; std::cin >> api;
    form->InitForm(fH, fW, "Runa Engine", api);
    GameObject* RootObject = new GameObject;
    SceneManager* scene = new SceneManager(RootObject, 720, 720);
    auto m_shader = scene->graphics_manager->CreateShader("D:/Github Repo/RunaEngine/assets/shaders/shader.vs", "D:/Github Repo/RunaEngine/assets/shaders/shader.ps");
    scene->graphics_manager->UseShader(m_shader);

    RootObject->AddComponent<MeshReference>(MeshImporter::ReadMesh("../assets/models/Aiz.obj"));
    // thisMesh = RootObject->AddComponent<MeshReference>(MeshImporter::ReadMesh("../assets/models/african_head.obj"));

    scene->InitSceneRenderable();
    while (form->DisplayFrame(0)) { 
      scene->RenderScene();
    }
  return 0;
}