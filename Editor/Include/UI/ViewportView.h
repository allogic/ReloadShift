#pragma once

#include <Core.h>

#include <Globals/World.h>

class ViewportView
{
public:

  static void Draw(bool& open)
  {
    World& world = World::Instance();
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar;
    for (auto const& [rendererName, renderer] : world.GetRenderer())
    {
      if (open)
      {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
        open = ImGui::Begin(rendererName.c_str(), &open, windowFlags);
        ImGui::PopStyleVar();
        ImGui::BeginMenuBar();

        // Make framebuffer attachments selectable
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
        ImGui::Image((void*)(U64)renderer->GetCurrentBufferID(), ImVec2{ renderer->GetViewportSize().x , renderer->GetViewportSize().y - 39.0f });
        ImGui::End();
      }
    }
  }
};
