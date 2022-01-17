#pragma once

#include <Core.h>

#include <Globals/World.h>

class ResourceView
{
public:

  static void Draw(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      ImGui::Begin("Resources", &open);
      if (ImGui::BeginTable("Resources", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg))
      {
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("FileSize");
        ImGui::TableSetupColumn("Dirty");
        ImGui::TableSetupColumn("FilePath");
        ImGui::TableHeadersRow();

        for (auto& [name, resource] : world.GetResources())
        {
          ImGui::TableNextRow();

          ImGui::TableNextColumn();
          ImGui::Text("%s", resource->GetType().c_str());

          ImGui::TableNextColumn();
          ImGui::Text(resource->GetName().c_str());

          ImGui::TableNextColumn();
          if (ImGui::Selectable(std::format("{}", (void*)resource).c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
          {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
          }

          ImGui::TableNextColumn();
          ImGui::Text("%u", resource->GetFileSize());

          ImGui::TableNextColumn();
          ImGui::Text("%u", resource->GetDirty());

          ImGui::TableNextColumn();
          ImGui::Text(resource->GetFilePath().string().c_str());
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
};