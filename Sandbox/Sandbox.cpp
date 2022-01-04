#define REDSHIFT_IMPLEMENTATION
#include <Redshift.h>

class Player : public Actor
{
public:

  Player(World* world, ActorProxy* proxy)
    : Actor(world, proxy)
    , mTransform{ world->Attach<Transform>(this, R32V3{ 0.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mRenderable{ world->Attach<Renderable>(this, "Puker")}
    , mCamera{ world->Attach<Camera>(this, 45.0f, 0.001f, 1000.0f) }
  {
    
  }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  Camera* mCamera;
};
class Enemy : public Actor
{
public:

  Enemy(World* world, ActorProxy* proxy)
    : Actor(world, proxy)
    , mTransform{ world->Attach<Transform>(this, R32V3{ 0.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mRenderable{ world->Attach<Renderable>(this, "Puker")}
  {

  }

private:

  Transform* mTransform;
  Renderable* mRenderable;
};

class Sandbox : public Module
{
public:

  Sandbox(World* world)
    : Module(world)
  {
    mUniformProjection = mWorld->CreateHandle<UniformBuffer<Projection>>("Projection", 1u);
  }

public:

  virtual void Tick(R32 deltaTime)
  {
    Module::Tick(deltaTime);

    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Begin("Debug");
    if (ImGui::Button("Add Player"))
    {
      mWorld->Create<Player>("Player");
    }
    if (ImGui::Button("Add Enemy"))
    {
      mWorld->Create<Enemy>("Enemy");
    }
    ImGui::End();
    
    ImGui::Begin("Resources");
    if (ImGui::BeginTable("Resources", 6))
    {
      ImGui::TableNextColumn(); ImGui::Text("Type");
      ImGui::TableNextColumn(); ImGui::Text("Address");
      ImGui::TableNextColumn(); ImGui::Text("Bytes");
      ImGui::TableNextColumn(); ImGui::Text("BytesSize");
      ImGui::TableNextColumn(); ImGui::Text("Dirty");
      ImGui::TableNextColumn(); ImGui::Text("FilePath");
      for (auto& [name, resource] : mWorld->GetResources())
      {
        ImGui::TableNextColumn(); ImGui::Text(name.c_str());
        ImGui::TableNextColumn(); ImGui::Text("%p", resource);
        ImGui::TableNextColumn(); ImGui::Text("%p", resource->GetBytes());
        ImGui::TableNextColumn(); ImGui::Text("%u", resource->GetBytesSize());
        ImGui::TableNextColumn(); ImGui::Text("%u", resource->GetDirty());
        ImGui::TableNextColumn(); ImGui::Text(resource->GetFilePath().string().c_str());
      }
      ImGui::EndTable();
    }
    ImGui::End();
    
    ImGui::Begin("Actors");
    if (ImGui::BeginTable("Actors", 5))
    {
      ImGui::TableNextColumn(); ImGui::Text("Name");
      ImGui::TableNextColumn(); ImGui::Text("Address");
      ImGui::TableNextColumn(); ImGui::Text("OuterMask");
      ImGui::TableNextColumn(); ImGui::Text("InnerMask");
      ImGui::TableNextColumn(); ImGui::Text("ComponentCount");
      for (auto const& [mask, proxy] : mWorld->GetActors())
      {
        ImGui::TableNextColumn(); ImGui::Text(proxy->GetActor()->GetName().c_str());
        ImGui::TableNextColumn(); ImGui::Text("%p", proxy->GetActor());
        ImGui::TableNextColumn(); ImGui::Text("%s", std::bitset<8>(mask).to_string().c_str());
        ImGui::TableNextColumn(); ImGui::Text("%s", std::bitset<8>(proxy->GetActor()->GetMask()).to_string().c_str());
        ImGui::TableNextColumn(); ImGui::Text("%u", 42);
      }
      ImGui::EndTable();
    }
    ImGui::End();
    
    ImGui::Begin("Handles");
    if (ImGui::BeginTable("Handles", 4))
    {
      ImGui::TableNextColumn(); ImGui::Text("Name");
      ImGui::TableNextColumn(); ImGui::Text("Address");
      ImGui::TableNextColumn(); ImGui::Text("Dirty");
      ImGui::TableNextColumn(); ImGui::Text("ReferenceCount");
      for (auto const& [name, handle] : mWorld->GetHandles())
      {
        ImGui::TableNextColumn(); ImGui::Text(name.c_str());
        ImGui::TableNextColumn(); ImGui::Text("%p", handle);
        ImGui::TableNextColumn(); ImGui::Text("%u", handle->GetDirty());
        ImGui::TableNextColumn(); ImGui::Text("%u", handle->GetReferenceCount());
      }
      ImGui::EndTable();
    }
    ImGui::End();
  }
  virtual void Render(R32 deltaTime) override
  {
    //mUniformProjectionLit->Bind();
    //mWorld->DispatchFor<
    //  Transform,
    //  Camera>([=](Transform* transform, Camera* camera)
    //    {
    //      mUniformProjectionLit->SetSingle(0, ProjectionLit{ glm::perspective(glm::radians(mCamera.FOV), 1280.0f / 720.0f, mCamera.Near, mCamera.Far), glm::lookAt(mCamera.Position, mCamera.Position + R32V3{ 0.0f, 0.0f, 1.0f }, R32V3{ 0.0f, 1.0f, 0.0f }), glm::identity<R32M4>() });
    //    });
    //mUniformProjectionLit->UnBind();
    //
    //mUniformProjectionLit->Bind();
    //mUniformProjectionLit->Mount(0);
    //mProgramLit->Bind();
    //mWorld->DispatchFor<
    //  Transform,
    //  Renderable>([=](Transform* transform, Renderable* renderable)
    //    {
    //      mTexturePukerAlbedo->Bind();
    //      mTexturePukerAlbedo->Mount(0);
    //      mMeshPuker->Bind();
    //      mMeshPuker->Draw();
    //    });
    //mMeshPuker->UnBind();
    //mTexturePukerAlbedo->UnBind();
    //mProgramLit->UnBind();
    //mUniformProjectionLit->UnBind();
  }

private:

  UniformBuffer<Projection>* mUniformProjection;
};

DECLARE_MODULE_IMPL(Sandbox)