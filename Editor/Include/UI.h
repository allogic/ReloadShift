#pragma once

#include <Core.h>
#include <Resource.h>
#include <EventRegistry.h>

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
    static bool openEventMapping = true;
    static bool openStringRegistry = true;

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
        if (ImGui::MenuItem("Event Mapping")) openEventMapping = true;
        if (ImGui::MenuItem("String Registry")) openStringRegistry = true;
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    DrawResourcesWindow(openResources);
    DrawActorsWindow(openActors);
    DrawHandlesWindow(openHandles);
    DrawViewportWindow(openViewport);

    DrawEventMapping(openEventMapping);
    DrawStringRegistry(openStringRegistry);
  }

  static void DrawResourcesWindow(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      ImGui::Begin("Resources", &open);
      if (ImGui::BeginTable("Resources", 7))
      {
        ImGui::TableNextColumn(); ImGui::Text("Type");
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Address");
        ImGui::TableNextColumn(); ImGui::Text("Bytes");
        ImGui::TableNextColumn(); ImGui::Text("BytesSize");
        ImGui::TableNextColumn(); ImGui::Text("Dirty");
        ImGui::TableNextColumn(); ImGui::Text("FilePath");
        for (auto& [name, resource] : world.GetResources())
        {
          if (resource)
          {
            ImGui::TableNextColumn(); ImGui::Text("%s", resource->GetType().c_str());
            ImGui::TableNextColumn(); ImGui::Text(resource->GetName().c_str());
            ImGui::TableNextColumn(); ImGui::Text("%p", resource);
            ImGui::TableNextColumn(); ImGui::Text("%p", resource->GetBytes());
            ImGui::TableNextColumn(); ImGui::Text("%u", resource->GetBytesSize());
            ImGui::TableNextColumn(); ImGui::Text("%u", resource->GetDirty());
            ImGui::TableNextColumn(); ImGui::Text(resource->GetFilePath().string().c_str());
          }
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
  static void DrawActorsWindow(bool& open)
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
        for (auto const& [name, proxy] : world.GetActors())
        {
          ImGui::TableNextColumn(); ImGui::Text(proxy->GetActor()->GetName().c_str());
          ImGui::TableNextColumn(); ImGui::Text("%p", proxy->GetActor());
          ImGui::TableNextColumn(); ImGui::Text("%s", std::bitset<8>(proxy->GetActor()->GetCurrentHash()).to_string().c_str());
          ImGui::TableNextColumn(); ImGui::Text("%u", proxy->GetActor()->GetComponentCount());
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
  static void DrawHandlesWindow(bool& open)
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
        for (auto const& [type, handlesByName] : world.GetHandles())
        {
          for (auto const& [name, hotRef] : handlesByName)
          {
            ImGui::TableNextColumn(); ImGui::Text(hotRef.Get() ? hotRef.Get()->GetType().c_str() : "<invalid>");
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
  static void DrawEventMapping(bool& open)
  {
    if (open)
    {
      EventRegistry& registry = World::Instance().GetEventRegistry();
      open = ImGui::Begin("Event Mapping", &open);
      if (ImGui::BeginTable("Axis", 3))
      {
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Instance");
        ImGui::TableNextColumn(); ImGui::Text("Delegate");
        for (auto const& [axisName, axisDelegate] : registry.GetAxisDelegates())
        {
          ImGui::TableNextColumn(); ImGui::Text(axisName.c_str());
          ImGui::TableNextColumn(); ImGui::Text("%p", axisDelegate.Instance);
          ImGui::TableNextColumn(); ImGui::Text("%p", axisDelegate.Delegate);
        }
        ImGui::EndTable();
      }
      if (ImGui::BeginTable("Action", 3))
      {
        ImGui::TableNextColumn(); ImGui::Text("Name");
        ImGui::TableNextColumn(); ImGui::Text("Instance");
        ImGui::TableNextColumn(); ImGui::Text("Delegate");
        for (auto const& [actionKey, actionsByType] : registry.GetActionDelegates())
        {
          for (auto const& [actionType, actionInfos] : actionsByType)
          {
            for (auto const& actionInfo : actionInfos)
            {
              ImGui::TableNextColumn(); ImGui::Text("%c", actionKey);
              ImGui::TableNextColumn(); ImGui::Text("%p", actionInfo.Instance);
              ImGui::TableNextColumn(); ImGui::Text("%p", actionInfo.Delegate);
            }
          }
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
  static void DrawStringRegistry(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      open = ImGui::Begin("String Registry", &open);
      if (ImGui::BeginTable("Strings", 2))
      {
        ImGui::TableNextColumn(); ImGui::Text("Key");
        ImGui::TableNextColumn(); ImGui::Text("Value");
        for (auto const& [key, value] : world.GetStringRegistry())
        {
          ImGui::TableNextColumn(); ImGui::Text(key.c_str());
          ImGui::TableNextColumn(); ImGui::Text(value.c_str());
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
};