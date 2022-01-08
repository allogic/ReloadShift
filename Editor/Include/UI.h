#pragma once

#include <Core.h>
#include <World.h>
#include <Resource.h>

class UI
{
public:

  static void Draw(World* world)
  {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    DrawResources(world);
    DrawActors(world);
    DrawHandles(world);
  }

private:

  static void DrawResources(World* world)
  {
    static bool opened = true;
    ImGui::Begin("Resources", &opened);
    if (ImGui::BeginTable("Resources", 7))
    {
      ImGui::TableNextColumn(); ImGui::Text("Type");
      ImGui::TableNextColumn(); ImGui::Text("Name");
      ImGui::TableNextColumn(); ImGui::Text("Address");
      ImGui::TableNextColumn(); ImGui::Text("Bytes");
      ImGui::TableNextColumn(); ImGui::Text("BytesSize");
      ImGui::TableNextColumn(); ImGui::Text("Dirty");
      ImGui::TableNextColumn(); ImGui::Text("FilePath");
      for (auto& [name, resource] : world->GetResources())
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
  static void DrawActors(World* world)
  {
    static bool opened = true;
    ImGui::Begin("Actors", &opened);
    if (ImGui::BeginTable("Actors", 4))
    {
      ImGui::TableNextColumn(); ImGui::Text("Name");
      ImGui::TableNextColumn(); ImGui::Text("Address");
      ImGui::TableNextColumn(); ImGui::Text("Hash");
      ImGui::TableNextColumn(); ImGui::Text("ComponentCount");
      for (auto const& [name, proxy] : world->GetActors())
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
  static void DrawHandles(World* world)
  {
    static bool opened = true;
    ImGui::Begin("Handles", &opened);
    if (ImGui::BeginTable("Handles", 5))
    {
      ImGui::TableNextColumn(); ImGui::Text("Type");
      ImGui::TableNextColumn(); ImGui::Text("Name");
      ImGui::TableNextColumn(); ImGui::Text("Address");
      ImGui::TableNextColumn(); ImGui::Text("Dirty");
      ImGui::TableNextColumn(); ImGui::Text("ReferenceCount");
      for (auto const& [type, handlesByName] : world->GetHandles())
      {
        for (auto const& [name, hotRef] : handlesByName)
        {
          ImGui::TableNextColumn(); ImGui::Text(hotRef.Get() ? hotRef.Get()->GetType().c_str() : "");
          ImGui::TableNextColumn(); ImGui::Text(name.c_str());
          ImGui::TableNextColumn(); ImGui::Text("%p", hotRef.Get());
          ImGui::TableNextColumn(); ImGui::Text("%u", hotRef.Get() ? hotRef.Get()->GetDirty() : 666);
          ImGui::TableNextColumn(); ImGui::Text("%u", hotRef.Get() ? hotRef.Get()->GetReferenceCount() : 666);
        }
      }
      ImGui::EndTable();
    }
    ImGui::End();
  }
};