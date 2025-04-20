#include "../Runtime/Core/eigen-3.4.0/Eigen/Eigen"
// #include "../Runtime/Core/Memory/MemoryManager.h"
#include "../Runtime/Forms/Win32/FormWin.h"
#include "../Runtime/Forms/imgui/imgui.h"
#include "../Runtime/Forms/imgui/backends/imgui_impl_opengl3.h"
#include "../Runtime/Forms/imgui/backends/imgui_impl_win32.h"
#include "../Runtime/Scene/GameObject.h"
#include "../Runtime/Scene/SceneManager.h"
#include "../Runtime/Scene/Mesh.h"
#include "src/Runtime/AssetsImport/MeshImporter.h"
#include "src/Runtime/Core/Memory/MemoryManager.h"
#include "src/Runtime/Scene/Components/MeshReference.h"
#include "src/Runtime/Scene/Components/ShaderReference.h"
#include "src/Runtime/Scene/Components/Transform.h"
#include "src/Runtime/Scene/PointLightSource.h"
#include <ctime>
#include <sysinfoapi.h>


uint32_t fW = 1600;
uint32_t fH = 900;
uint32_t gW = 1280, gH = 720;

// const uint32_t frametime = 2;

std::string model_path[] = {"../assets/models/Aiz.obj", "../assets/models/african_head.obj"};

int main()
{
  FormWin* form = MemoryManager::New<FormWin>();
  form->InitForm(fW, fH, "Runa Engine", FORM_RHI_OPENGL);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.FontGlobalScale = 1.0f;
  io.Fonts->AddFontFromFileTTF("../assets/fonts/arial.ttf", 15.0f);
  ImGui::StyleColorsDark();
  
  ImGui_ImplWin32_InitForOpenGL(form->getHwnd());
  ImGui_ImplOpenGL3_Init();

  GameObject* RootObject = MemoryManager::New<GameObject>();
  SceneManager* scene = MemoryManager::New<SceneManager>(RootObject, gW, gH);

  GameObject* go = MemoryManager::New<GameObject>();
  scene->AddNewGameObject(go);
  uint32_t model_idx = 0;

  go->AddComponent<MeshReference>(MeshImporter::ReadMesh(model_path[model_idx]));
  go->AddComponent<ShaderReference>();

  GameObject* grid = MemoryManager::New<GameObject>();
  scene->AddNewGameObject(grid);
  grid->AddComponent<MeshReference>(MeshImporter::ReadMesh("../assets/models/test.obj"));
  grid->AddComponent<ShaderReference>(MemoryManager::New<ShaderReference>("../assets/shaders/gridshader.vs", "../assets/shaders/gridshader.ps"));

  GameObject* cam = MemoryManager::New<Camera>(0, 0, -1.0f, 0, 1.0f, 2.0f, 90.0f);
  ((Camera*)cam)->aspect = 1.0f * gW / gH;
  scene->AddNewGameObject(cam);
  scene->SetMainCamera((Camera*)cam);

  LightSource* ls = MemoryManager::New<PointLightSource>(1.0f, Eigen::Vector3f(0.3f, 0.0f, -0.6f));
  scene->AddLightSource(ls);
  scene->InitSceneRenderable();

  ImVec2 window_pos(gW, 0);
  auto t1 = clock();

  while (form->DisplayFrame()) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(ImVec2(fW - gW, fW - 20));
    {
          auto t2 = clock();
          auto delta_time = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
          t1 = t2;
          static float f = 0.0f;

          ImGui::Begin("Hello, Runa Engine!", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);                          // Create a window called "Hello, world!" and append into it.
          ImGui::SliderFloat("Light Intensity", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

          if (ImGui::Button("Change Model")) {
            scene->Destroy(go);
            model_idx ^= 1;

            auto gos = MemoryManager::New<GameObject>();
            gos->AddComponent<MeshReference>(MeshImporter::ReadMesh(model_path[model_idx]));
            gos->AddComponent<ShaderReference>();
            // if (model_idx != 1)
            //   gos->AddComponent<ShaderReference>();
            // else
            //   gos->AddComponent<ShaderReference>(new ShaderReference("../assets/shaders/gridshader.vs", "../assets/shaders/gridshader.ps"));
            scene->AddNewGameObject(gos);
            scene->InitSingleSceneRenderable(gos);
            go = gos;
          }
          auto tf = dynamic_cast<Transform*>(cam->GetComponent<Transform>());
          auto tfgo = dynamic_cast<Transform*>(go->GetComponent<Transform>());
          if (ImGui::IsKeyDown(ImGuiKey_W)) {
            tf->position() += dynamic_cast<Camera*>(cam)->camDirection * delta_time * 3.0f;
          }
          if (ImGui::IsKeyDown(ImGuiKey_S)) {
            tf->position() -= dynamic_cast<Camera*>(cam)->camDirection * delta_time * 3.0f;
          }
          if (ImGui::IsKeyDown(ImGuiKey_A)) {
            tf->position() -= dynamic_cast<Camera*>(cam)->camRight() * delta_time * 3.0f;
          }
          if (ImGui::IsKeyDown(ImGuiKey_D)) {
            tf->position() += dynamic_cast<Camera*>(cam)->camRight() * delta_time * 3.0f;
          }
          if (ImGui::IsKeyDown(ImGuiKey_Space)) {
            tf->position() += dynamic_cast<Camera*>(cam)->camUp * delta_time * 3.0f;
          }
          if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            tf->position() -= dynamic_cast<Camera*>(cam)->camUp * delta_time * 3.0f;
          }
          if (ImGui::IsKeyDown(ImGuiKey_Q)) {
            tfgo->rotation().y() += delta_time * 3.0f;
          }
          if (ImGui::IsKeyDown(ImGuiKey_E)) {
            tfgo->rotation().y() -= delta_time * 3.0f;
          }

          ImGui::SameLine();
          ImGui::Text("Current Model Path:\n%s", model_path[model_idx].c_str());

          if (ImGui::Button("Change Shader")) {
            
          }
          ImGui::SameLine();
          ImGui::Text("Current Shader: \n%s", dynamic_cast<ShaderReference*>(go->GetComponent<ShaderReference>())->vpath.c_str());
          

          ImGui::Text("Average FPS: %.1f", io.Framerate);
          ImGui::End();
      }
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(gW, fH - gH));
      {
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::End();
      }
    ImGui::Render();
    scene->RenderScene();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
  
  return 0;
}