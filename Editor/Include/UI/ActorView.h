#pragma once

#include <Core.h>

#include <Globals/World.h>

class ActorView
{
public:

  static void Draw(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      ImGui::Begin("Actors", &open);
      if (ImGui::BeginTable("Actors", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg))
      {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Hash");
        ImGui::TableSetupColumn("ComponentCount");
        ImGui::TableHeadersRow();

        // Store new selected actor
        Actor* selectedActor = nullptr;
        for (auto const& [actor, proxy] : world.GetActors())
        {
          ImGui::TableNextRow();

          ImGui::TableNextColumn();
          ImGui::Text("%s", actor->GetName().c_str());

          ImGui::TableNextColumn();
          if (ImGui::Selectable(std::format("{}", (void*)actor).c_str(), actor->GetSelected(), ImGuiSelectableFlags_SpanAllColumns))
          {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);
            actor->SetSelected(true);
            selectedActor = actor;
          }

          ImGui::TableNextColumn();
          ImGui::Text("%s", std::bitset<64>(actor->GetCurrentHash()).to_string().c_str());

          ImGui::TableNextColumn();
          ImGui::Text("%u", actor->GetComponentCount());
        }

        // Set all other actors to not selected
        if (selectedActor)
        {
          for (auto const& [actor, proxy] : world.GetActors())
          {
            if (actor != selectedActor)
            {
              actor->SetSelected(false);
            }
          }
        }

        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
};