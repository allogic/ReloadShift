#pragma once

#include <Core.h>

#include <Globals/World.h>

#include <Components/Camera.h>
#include <Components/Renderable.h>
#include <Components/Rigidbody.h>
#include <Components/Transform.h>

#include <Components/Collider/BoxCollider.h>
#include <Components/Collider/SphereCollider.h>

#include <Components/Lights/AmbientLight.h>
#include <Components/Lights/DirectionLight.h>
#include <Components/Lights/PointLight.h>
#include <Components/Lights/SpotLight.h>

class PropertyView
{
public:

  static void Draw(bool& open)
  {
    if (open)
    {
      World& world = World::Instance();
      ImGui::Begin("Properties", &open);
      for (auto const& [actor, proxy] : world.GetActors())
      {
        if (actor->GetSelected())
        {
          if (Transform* transform = actor->GetComponent<Transform>())
          {
            if (ImGui::CollapsingHeader("Transform"))
            {
              ImGui::DragFloat3("Position", &transform->GetWorldPosition()[0]);
              ImGui::DragFloat3("Rotation", &transform->GetWorldRotation()[0]);
              ImGui::DragFloat3("Scale", &transform->GetWorldScale()[0]);
            }
          }
          if (Camera* camera = actor->GetComponent<Camera>())
          {
            if (ImGui::CollapsingHeader("Camera"))
            {
              ImGui::DragFloat("FOV", &camera->GetFOV());
              ImGui::DragFloat("Near", &camera->GetNear());
              ImGui::DragFloat("Far", &camera->GetFar());
            }
          }
          if (Renderable* renderable = actor->GetComponent<Renderable>())
          {
            if (ImGui::CollapsingHeader("Renderable"))
            {
              ImGui::Text("Mesh: %s", renderable->GetMesh() ? renderable->GetMesh()->GetName().c_str() : "");
              ImGui::Text("Program: %s", renderable->GetProgram() ? renderable->GetProgram()->GetName().c_str() : "");
              ImGui::Text("Albedo: %s", renderable->GetTextureAlbedo() ? renderable->GetTextureAlbedo()->GetName().c_str() : "");
              ImGui::Text("Normal: %s", renderable->GetTextureNormal() ? renderable->GetTextureNormal()->GetName().c_str() : "");
              ImGui::Text("Specular: %s", renderable->GetTextureSpecular() ? renderable->GetTextureSpecular()->GetName().c_str() : "");
              ImGui::Text("Metallic: %s", renderable->GetTextureMetallic() ? renderable->GetTextureMetallic()->GetName().c_str() : "");
              ImGui::Text("Roughness: %s", renderable->GetTextureRoughness() ? renderable->GetTextureRoughness()->GetName().c_str() : "");
              ImGui::Text("AmbientOcclusion: %s", renderable->GetTextureAmbientOcclusion() ? renderable->GetTextureAmbientOcclusion()->GetName().c_str() : "");
            }
          }
          if (Rigidbody* rigidbody = actor->GetComponent<Rigidbody>())
          {
            if (ImGui::CollapsingHeader("Rigidbody"))
            {
              
            }
          }

          if (BoxCollider* boxCollider = actor->GetComponent<BoxCollider>())
          {
            if (ImGui::CollapsingHeader("Box Collider"))
            {
              
            }
          }
          if (SphereCollider* sphereCollider = actor->GetComponent<SphereCollider>())
          {
            if (ImGui::CollapsingHeader("Sphere Collider"))
            {

            }
          }

          if (AmbientLight* ambientLight = actor->GetComponent<AmbientLight>())
          {
            if (ImGui::CollapsingHeader("Ambient Light"))
            {
              ImGui::DragFloat("Intensity", &ambientLight->GetIntensity());
              ImGui::ColorPicker3("Color", &ambientLight->GetColor()[0]);
              ImGui::Checkbox("Enabled", &ambientLight->GetEnabled());
            }
          }
          if (DirectionLight* directionLight = actor->GetComponent<DirectionLight>())
          {
            if (ImGui::CollapsingHeader("Direction Light"))
            {
              ImGui::DragFloat("Intensity", &directionLight->GetIntensity());
              ImGui::ColorPicker3("Color", &directionLight->GetColor()[0]);
              ImGui::Checkbox("Enabled", &directionLight->GetEnabled());
            }
          }
          if (PointLight* pointLight = actor->GetComponent<PointLight>())
          {
            if (ImGui::CollapsingHeader("Point Light"))
            {
              ImGui::DragFloat("Intensity", &pointLight->GetIntensity());
              ImGui::ColorPicker3("Color", &pointLight->GetColor()[0]);
              ImGui::Checkbox("Enabled", &pointLight->GetEnabled());
            }
          }
          if (SpotLight* spotLight = actor->GetComponent<SpotLight>())
          {
            if (ImGui::CollapsingHeader("Spot Light"))
            {
              ImGui::DragFloat("Intensity", &spotLight->GetIntensity());
              ImGui::DragFloat("CutOff", &spotLight->GetCutOff());
              ImGui::ColorPicker3("Color", &spotLight->GetColor()[0]);
              ImGui::Checkbox("Enabled", &spotLight->GetEnabled());
            }
          }
        }
      }
      ImGui::End();
    }
  }
};