#pragma once

#include <Core.h>

#include <Globals/World.h>

class DelegateView
{
public:

  static void Draw(bool& open)
  {
    World& world = World::Instance();
    Delegates& registry = world.GetDelegates();
    if (open)
    {
      open = ImGui::Begin("Delegates", &open);

      if (ImGui::CollapsingHeader("Axis"))
      {
        if (ImGui::BeginTable("Axis", 3))
        {
          ImGui::TableSetupColumn("Name");
          ImGui::TableSetupColumn("Instance");
          ImGui::TableSetupColumn("Delegate");
          ImGui::TableHeadersRow();

          for (auto const& [axisName, axisInfos] : registry.GetAxisDelegates())
          {
            for (auto const& axisInfo : axisInfos)
            {
              ImGui::TableNextRow();

              ImGui::TableNextColumn();
              ImGui::Text(axisName.c_str());

              ImGui::TableNextColumn();
              if (ImGui::Selectable(std::format("{}", (void*)axisInfo.Instance).c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
              {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
              }

              ImGui::TableNextColumn();
              ImGui::Text("%p", axisInfo.Delegate);
            }
          }
          ImGui::EndTable();
        }
      }

      if (ImGui::CollapsingHeader("Actions"))
      {
        if (ImGui::BeginTable("Actions", 3))
        {
          ImGui::TableSetupColumn("Name");
          ImGui::TableSetupColumn("Instance");
          ImGui::TableSetupColumn("Delegate");
          ImGui::TableHeadersRow();

          for (auto const& [actionKey, actionInfos] : registry.GetActionDelegates())
          {
            for (auto const& actionInfo : actionInfos)
            {
              ImGui::TableNextRow();

              ImGui::TableNextColumn();
              ImGui::Text("%u", actionKey.Key);

              ImGui::TableNextColumn();
              if (ImGui::Selectable(std::format("{}", (void*)actionInfo.Instance).c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
              {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
              }

              ImGui::TableNextColumn();
              ImGui::Text("%p", actionInfo.Delegate);
            }
          }
          ImGui::EndTable();
        }
      }

      ImGui::End();
    }
  }
};