#include "../Runtime/Core/eigen-3.4.0/Eigen/Eigen"
#include "../Runtime/Core/Memory/MemoryManager.h"
#include "../Runtime/Forms/Win32/FormWin.h"
#include "../Runtime/Scene/GameObject.h"
#include "../Runtime/Scene/SceneManager.h"
#include "../Runtime/Scene/Mesh.h"
#include "src/Runtime/AssetsImport/MeshImporter.h"
#include "src/Runtime/Scene/Components/MeshReference.h"
#include "src/Runtime/Scene/Components/ShaderReference.h"
#include <iostream>

uint32_t fH = 1280;
uint32_t fW = 720;
uint32_t gH = 800, gW = 600;
int main()
{
  // freopen("out.txt", "w", stdout);
    Form* form = dynamic_cast<FormWin*>(MemoryManager::GetInstance()->New<FormWin>());
    std::cout << "Select API you want. 1 for OpenGL, 2 for DirectX 11." << std::endl;
    int api; std::cin >> api;
    form->InitForm(fH, fW, "Runa Engine", api);
    GameObject* RootObject = new GameObject;
    SceneManager* scene = new SceneManager(RootObject, gH, gW);

    RootObject->AddComponent<MeshReference>(MeshImporter::ReadMesh("../assets/models/Aiz.obj"));
    RootObject->AddComponent<ShaderReference>();

    // GameObject* African = new GameObject;
    // African->AddComponent<MeshReference>(MeshImporter::ReadMesh("../assets/models/african_head.obj"));
    // African->AddComponent<ShaderReference>();

    // scene->AddNewGameObject(African);

    GameObject* cam = new Camera(0, 0, -1.0f, 0, 1, 2.0f, 90.0f);
    
    scene->AddNewGameObject(cam);
    scene->SetMainCamera((Camera*)cam);

    scene->InitSceneRenderable();
    while (form->DisplayFrame()) { 
      scene->RenderScene();
    }
  return 0;
}