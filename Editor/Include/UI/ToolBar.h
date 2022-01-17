#pragma once

#include <Core.h>

#include <Ui/ActorView.h>
#include <Ui/DelegateView.h>
#include <Ui/HandleView.h>
#include <Ui/PropertyView.h>
#include <Ui/RegistryView.h>
#include <Ui/ResourceView.h>
#include <Ui/ViewportView.h>

class ToolBar
{
public:

  static void Draw()
  {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    
    static bool openActors = true;
    static bool openDelegates = true;
    static bool openHandles = true;
    static bool openProperties = true;
    static bool openRegistry = true;
    static bool openResources = true;
    static bool openViewports = true;

    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("Windows"))
      {
        if (ImGui::MenuItem("Actors")) openActors = true;
        if (ImGui::MenuItem("Delegates")) openDelegates = true;
        if (ImGui::MenuItem("Handles")) openHandles = true;
        if (ImGui::MenuItem("Properties")) openProperties = true;
        if (ImGui::MenuItem("Registry")) openRegistry = true;
        if (ImGui::MenuItem("Resources")) openResources = true;
        if (ImGui::MenuItem("Viewports")) openViewports = true;
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    ActorView::Draw(openActors);
    DelegateView::Draw(openDelegates);
    HandleView::Draw(openHandles);
    PropertyView::Draw(openProperties);
    RegistryView::Draw(openRegistry);
    ResourceView::Draw(openResources);
    ViewportView::Draw(openViewports);
  }
};