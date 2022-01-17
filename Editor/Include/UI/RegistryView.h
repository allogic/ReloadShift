#pragma once

#include <Core.h>

#include <Globals/Registry.h>

class RegistryView
{
public:

  static void Draw(bool& open)
  {
    Registry& registry = Registry::Instance();
    if (open)
    {
      open = ImGui::Begin("Registry", &open);

      if (ImGui::CollapsingHeader("I32"))
      {
        if (ImGui::BeginTable("Registry", 3))
        {
          ImGui::TableSetupColumn("Type");
          ImGui::TableSetupColumn("Name");
          ImGui::TableSetupColumn("Value");
          ImGui::TableHeadersRow();

          for (auto const& [key, value] : registry.GetRegistryFor<I32>())
          {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("I32");

            ImGui::TableNextColumn();
            if (ImGui::Selectable(key.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
            }

            ImGui::TableNextColumn();
            ImGui::Text("%d", value);
          }
          ImGui::EndTable();
        }
      }

      if (ImGui::CollapsingHeader("U32"))
      {
        if (ImGui::BeginTable("Registry", 3))
        {
          ImGui::TableSetupColumn("Type");
          ImGui::TableSetupColumn("Name");
          ImGui::TableSetupColumn("Value");
          ImGui::TableHeadersRow();

          for (auto const& [key, value] : registry.GetRegistryFor<U32>())
          {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("U32");

            ImGui::TableNextColumn();
            if (ImGui::Selectable(key.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
            }

            ImGui::TableNextColumn();
            ImGui::Text("%u", value);
          }
          ImGui::EndTable();
        }
      }

      if (ImGui::CollapsingHeader("R32"))
      {
        if (ImGui::BeginTable("Registry", 3))
        {
          ImGui::TableSetupColumn("Type");
          ImGui::TableSetupColumn("Name");
          ImGui::TableSetupColumn("Value");
          ImGui::TableHeadersRow();

          for (auto const& [key, value] : registry.GetRegistryFor<R32>())
          {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("R32");

            ImGui::TableNextColumn();
            if (ImGui::Selectable(key.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
              ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
            }

            ImGui::TableNextColumn();
            ImGui::Text("%f", value);
          }
          ImGui::EndTable();
        }
      }

      ImGui::End();
    }
  }
};
