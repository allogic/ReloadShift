#pragma once

#include <Core.h>
#include <Resource.h>
#include <Delegates.h>

#include <Globals/World.h>

class UI
{
public:

  static void Draw()
  {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    DrawToolBar();
  }

private:

  static void DrawToolBar()
  {
    static bool openResources = true;
    static bool openActors = true;
    static bool openHandles = true;
    static bool openViewport = true;
    static bool openDelegates = true;
    static bool openRegistry = true;

    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("Physics"))
      {
        static R32V3 gravity = {};
        ImGui::InputFloat3("Gravity", &gravity[0]);
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Windows"))
      {
        if (ImGui::MenuItem("Resources")) openResources = true;
        if (ImGui::MenuItem("Actors")) openActors = true;
        if (ImGui::MenuItem("Handles")) openHandles = true;
        if (ImGui::MenuItem("Viewport")) openViewport = true;
        if (ImGui::MenuItem("Delegates")) openDelegates = true;
        if (ImGui::MenuItem("Registry")) openRegistry = true;
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    DrawResourceWindow(openResources);
    DrawActorWindow(openActors);
    DrawHandleWindow(openHandles);
    DrawViewportWindow(openViewport);
    DrawDelegateWindow(openDelegates);
    DrawRegistryWindow(openRegistry);
  }

  static void DrawResourceWindow(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      ImGui::Begin("Resources", &open);
      if (ImGui::BeginTable("Resources", 6))
      {
        ImGui::TableNextColumn(); ImGui::Text("Type");
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Address");
        ImGui::TableNextColumn(); ImGui::Text("FileSize");
        ImGui::TableNextColumn(); ImGui::Text("Dirty");
        ImGui::TableNextColumn(); ImGui::Text("FilePath");
        for (auto& [name, resource] : world.GetResources())
        {
          if (resource)
          {
            ImGui::TableNextColumn(); ImGui::Text("%s", resource->GetType().c_str());
            ImGui::TableNextColumn(); ImGui::Text(resource->GetName().c_str());
            ImGui::TableNextColumn(); ImGui::Text("%p", resource);
            ImGui::TableNextColumn(); ImGui::Text("%u", resource->GetFileSize());
            ImGui::TableNextColumn(); ImGui::Text("%u", resource->GetDirty());
            ImGui::TableNextColumn(); ImGui::Text(resource->GetFilePath().string().c_str());
          }
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
  static void DrawActorWindow(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      ImGui::Begin("Actors", &open);
      if (ImGui::BeginTable("Actors", 4))
      {
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Address");
        ImGui::TableNextColumn(); ImGui::Text("Hash");
        ImGui::TableNextColumn(); ImGui::Text("ComponentCount");
        for (auto const& [actor, proxy] : world.GetActors())
        {
          ImGui::TableNextColumn(); ImGui::Text(actor->GetName().c_str());
          ImGui::TableNextColumn(); ImGui::Text("%p", actor);
          ImGui::TableNextColumn(); ImGui::Text("%s", std::bitset<8>(actor->GetCurrentHash()).to_string().c_str());
          ImGui::TableNextColumn(); ImGui::Text("%u", actor->GetComponentCount());
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
  static void DrawHandleWindow(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      open = ImGui::Begin("Handles", &open);
      if (ImGui::BeginTable("Handles", 5))
      {
        ImGui::TableNextColumn(); ImGui::Text("Type");
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Address");
        ImGui::TableNextColumn(); ImGui::Text("Dirty");
        ImGui::TableNextColumn(); ImGui::Text("ReferenceCount");
        for (auto const& [type, hotRefs] : world.GetHandles())
        {
          for (auto const& [name, hotRef] : hotRefs)
          {
            ImGui::TableNextColumn(); ImGui::Text(hotRef.Get() ? hotRef.Get()->GetType().c_str() : "");
            ImGui::TableNextColumn(); ImGui::Text(name.c_str());
            ImGui::TableNextColumn(); ImGui::Text("%p", hotRef.Get());
            ImGui::TableNextColumn(); ImGui::Text("%d", hotRef.Get() ? hotRef.Get()->GetDirty() : -1);
            ImGui::TableNextColumn(); ImGui::Text("%d", hotRef.Get() ? hotRef.Get()->GetReferenceCount() : -1);
          }
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
  static void DrawViewportWindow(bool& open)
  {
    World& world = World::Instance();
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar;
    for (auto const& [rendererName, renderer] : world.GetRenderer())
    {
      if (open)
      {
        bool resize = false;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
        open = ImGui::Begin(rendererName.c_str(), &open, windowFlags);
        ImGui::PopStyleVar();
        ImGui::BeginMenuBar();
        if (ImGui::BeginMenu("Buffers"))
        {
          for (auto const& [bufferName, bufferID] : renderer->GetBuffers())
          {
            if (ImGui::MenuItem(bufferName.c_str())) renderer->SetCurrentBufferID(bufferID);
          }
          ImGui::EndMenu();
        }
        renderer->SetViewportSize(R32V2{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y });
        ImGui::Text("%ux%u", (U32)renderer->GetViewportSize().x, (U32)renderer->GetViewportSize().y);
        ImGui::EndMenuBar();
        ImGui::Image((void*)(U64)renderer->GetCurrentBufferID(), ImVec2{renderer->GetViewportSize().x , renderer->GetViewportSize().y - 39.0f});
        ImGui::End();
      }
    }
  }
  static void DrawDelegateWindow(bool& open)
  {
    World& world = World::Instance();
    Delegates& registry = world.GetDelegates();
    if (open)
    {
      open = ImGui::Begin("Delegates", &open);
      if (ImGui::BeginTable("Axis", 3))
      {
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Instance");
        ImGui::TableNextColumn(); ImGui::Text("Delegate");
        for (auto const& [axisName, axisInfos] : registry.GetAxisDelegates())
        {
          for (auto const& axisInfo : axisInfos)
          {
            ImGui::TableNextColumn(); ImGui::Text(axisName.c_str());
            ImGui::TableNextColumn(); ImGui::Text("%p", axisInfo.Instance);
            ImGui::TableNextColumn(); ImGui::Text("%p", axisInfo.Delegate);
          }
        }
        ImGui::EndTable();
      }
      if (ImGui::BeginTable("Actions", 3))
      {
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Instance");
        ImGui::TableNextColumn(); ImGui::Text("Delegate");
        for (auto const& [actionKey, actionInfos] : registry.GetActionDelegates())
        {
          for (auto const& actionInfo : actionInfos)
          {
            ImGui::TableNextColumn(); ImGui::Text("%u", actionKey.Key);
            ImGui::TableNextColumn(); ImGui::Text("%p", actionInfo.Instance);
            ImGui::TableNextColumn(); ImGui::Text("%p", actionInfo.Delegate);
          }
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
  static void DrawRegistryWindow(bool& open)
  {
    Registry& registry = Registry::Instance();
    if (open)
    {
      open = ImGui::Begin("Registry", &open);
      if (ImGui::BeginTable("Registry", 3))
      {
        ImGui::TableNextColumn(); ImGui::Text("Type");
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Value");
        for (auto const& [key, value] : registry.GetRegistryFor<I32>())
        {
          ImGui::TableNextColumn(); ImGui::Text("I32");
          ImGui::TableNextColumn(); ImGui::Text(key.c_str());
          ImGui::TableNextColumn(); ImGui::Text("%d", value);
        }
        for (auto const& [key, value] : registry.GetRegistryFor<U32>())
        {
          ImGui::TableNextColumn(); ImGui::Text("U32");
          ImGui::TableNextColumn(); ImGui::Text(key.c_str());
          ImGui::TableNextColumn(); ImGui::Text("%u", value);
        }
        for (auto const& [key, value] : registry.GetRegistryFor<R32>())
        {
          ImGui::TableNextColumn(); ImGui::Text("R32");
          ImGui::TableNextColumn(); ImGui::Text(key.c_str());
          ImGui::TableNextColumn(); ImGui::Text("%f", value);
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
};