#pragma once

#include <Core.h>

#include <Globals/World.h>

class HandleView
{
public:

  static void Draw(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      open = ImGui::Begin("Handles", &open);      
      for (auto const& [type, hotRefs] : world.GetHandles())
      {
        if (ImGui::CollapsingHeader(std::to_string(type).c_str()))
        {
          if (ImGui::BeginTable("Handles", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg))
          {
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Dirty");
            ImGui::TableSetupColumn("ReferenceCount");
            ImGui::TableHeadersRow();

            for (auto const& [name, hotRef] : hotRefs)
            {
              ImGui::TableNextRow();

              ImGui::TableNextColumn();
              ImGui::Text(hotRef.Get() ? hotRef.Get()->GetType().c_str() : "");

              ImGui::TableNextColumn();
              ImGui::Text(name.c_str());

              ImGui::TableNextColumn();
              if (ImGui::Selectable(std::format("{}", (void*)hotRef.Get()).c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
              {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
              }

              ImGui::TableNextColumn();
              ImGui::Text("%d", hotRef.Get() ? hotRef.Get()->GetDirty() : -1);

              ImGui::TableNextColumn();
              ImGui::Text("%d", hotRef.Get() ? hotRef.Get()->GetReferenceCount() : -1);
            }

            ImGui::EndTable();
          }
        }
      }
      ImGui::End();
    }
  }
};