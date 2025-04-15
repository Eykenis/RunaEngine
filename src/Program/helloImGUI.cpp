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
#include "src/Runtime/Scene/Components/MeshReference.h"
#include "src/Runtime/Scene/Components/ShaderReference.h"
#include "src/Runtime/Scene/Components/Transform.h"
#include <ctime>
#include <sysinfoapi.h>


uint32_t fW = 1280;
uint32_t fH = 720;
uint32_t gW = 800, gH = 600;

const uint32_t FPS = 60;

std::string model_path[] = {"../assets/models/Aiz.obj", "../assets/models/african_head.obj"};

int main()
{
  FormWin* form = dynamic_cast<FormWin*>(MemoryManager::GetInstance()->New<FormWin>());
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

  GameObject* RootObject = new GameObject;
  SceneManager* scene = new SceneManager(RootObject, gW, gH);

  GameObject* go = new GameObject;
  scene->AddNewGameObject(go);
  go->AddComponent<MeshReference>(MeshImporter::ReadMesh("../assets/models/Aiz.obj"));
  go->AddComponent<ShaderReference>();

  uint32_t model_idx = 0;

  GameObject* cam = new Camera(0, 0, -1.0f, 0, 0, 2.0f, 90.0f);
  ((Camera*)cam)->aspect = 1.0f * gW / gH;
  scene->AddNewGameObject(cam);
  scene->SetMainCamera((Camera*)cam);
  scene->InitSceneRenderable();

  ImVec2 window_pos(gW, 0);
  auto t1 = clock();

  while (form->DisplayFrame(0)) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(ImVec2(fW - gW, fW - 20));
    {
          auto t2 = clock();
          // if (t2 - t1 < 1.0 * CLOCKS_PER_SEC / FPS) {
          //   Sleep(1.0 * CLOCKS_PER_SEC / FPS - t2 + t1);
          // }
          auto delta_time = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
          t1 = t2;
          static float f = 0.0f;

          ImGui::Begin("Hello, Runa Engine!", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);                          // Create a window called "Hello, world!" and append into it.
          ImGui::SliderFloat("Light Intensity", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

          if (ImGui::Button("Change Model")) {
            scene->Destroy(go);
            model_idx ^= 1;

            auto gos = new GameObject;
            gos->AddComponent<MeshReference>(MeshImporter::ReadMesh(model_path[model_idx]));
            gos->AddComponent<ShaderReference>();
            scene->AddNewGameObject(gos);
            scene->InitSingleSceneRenderable(gos);
            go = gos;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_W)) {
            auto tf = dynamic_cast<Transform*>(cam->GetComponent<Transform>());
            tf->position() += dynamic_cast<Camera*>(cam)->camDirection * delta_time * 5.0f;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_S)) {
            auto tf = dynamic_cast<Transform*>(cam->GetComponent<Transform>());
            tf->position() -= dynamic_cast<Camera*>(cam)->camDirection * delta_time * 5.0f;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_A)) {
            auto tf = dynamic_cast<Transform*>(cam->GetComponent<Transform>());
            tf->position() -= dynamic_cast<Camera*>(cam)->camRight() * delta_time * 5.0f;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_D)) {
            auto tf = dynamic_cast<Transform*>(cam->GetComponent<Transform>());
            tf->position() += dynamic_cast<Camera*>(cam)->camRight() * delta_time * 5.0f;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
            auto tf = dynamic_cast<Transform*>(cam->GetComponent<Transform>());
            tf->position() += dynamic_cast<Camera*>(cam)->camUp * delta_time * 5.0f;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_LeftShift)) {
            auto tf = dynamic_cast<Transform*>(cam->GetComponent<Transform>());
            tf->position() -= dynamic_cast<Camera*>(cam)->camUp * delta_time * 5.0f;
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
        ImGui::Text("%f", ImGui::GetWindowHeight());
        ImGui::End();
      }
    ImGui::Render();
    scene->RenderScene();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
  
  return 0;
}