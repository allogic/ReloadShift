#pragma once

#include <Core.h>
#include <Renderer.h>

class DeferredRenderer : public Renderer
{
public:

  using FrameBuffer = FrameBufferU24U8DS<
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA>;

public:

  DeferredRenderer(World* world)
    : Renderer(world)
    , mUniformProjection{ world->MountHandle<UniformBuffer<Projection>>("Projection", 1u) }
    , mDeferredDepthStencil{ world->MountHandle<Texture2DU24U8DS>("DeferredDepthStencil", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mDeferredPosition{ world->MountHandle<Texture2DR32RGBA>("DeferredPosition", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mDeferredAlbedo{ world->MountHandle<Texture2DR32RGBA>("DeferredAlbedo", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mDeferredNormal{ world->MountHandle<Texture2DR32RGBA>("DeferredNormal", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mDeferredSpecular{ world->MountHandle<Texture2DR32RGBA>("DeferredSpecular", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mDeferredHeight{ world->MountHandle<Texture2DR32RGBA>("DeferredHeight", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mDeferredMetallic{ world->MountHandle<Texture2DR32RGBA>("DeferredMetallic", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mDeferredRoughness{ world->MountHandle<Texture2DR32RGBA>("DeferredRoughness", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mFrameBuffer{ world->MountHandle<FrameBuffer>("Deferred", 1280u, 720u, mDeferredDepthStencil, mDeferredPosition, mDeferredAlbedo, mDeferredNormal, mDeferredSpecular, mDeferredHeight, mDeferredMetallic, mDeferredRoughness) }
  {
    
  }

public:

  virtual void Render() override
  {
    OPTICK_FRAME("DeferredRender");
    // Submit render tasks for all renderables
    mWorld->DispatchFor<
      Transform,
      Renderable>([=](Transform* transform, Renderable* renderable)
        {
          renderable->SubmitDrawCall(transform, mRenderQueue); // Template me?
        });
    // Bind uniforms
    mUniformProjection->Bind();
    mUniformProjection->Mount(0);
    // Update projection uniform
    mWorld->DispatchFor<
      Transform,
      Camera>([&](Transform* transform, Camera* camera)
        {
          mProjection.Projection = glm::perspective(glm::radians(camera->GetFOV()), 1280.0f / 720.0f, camera->GetNear(), camera->GetFar());
          mProjection.View = glm::lookAt(transform->GetPosition(), transform->GetPosition() + R32V3{ 0.0f, 0.0f, 1.0f }, R32V3{ 0.0f, 1.0f, 0.0f });
        });
    // Render geometry into framebuffer
    mFrameBuffer->BindWrite();
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GeometryPass();
    glDisable(GL_BLEND);
    mFrameBuffer->UnBind();
    // Blend framebuffer with background
    //mFrameBuffer->BindRead();
    //glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    //mFrameBuffer->UnBind();
    // UnBind uniforms
    mUniformProjection->UnBind();
  }

public:

  void Debug() const
  {
    ImGuiWindowFlags windowsFlags = ImGuiWindowFlags_NoDecoration;
    static bool opened = true;

    ImGui::Begin("DepthStencil", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredDepthStencil->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Position", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredPosition->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Albedo", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredAlbedo->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Normal", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredNormal->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Specular", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredSpecular->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Height", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredHeight->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Metallic", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredMetallic->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Roughness", &opened, windowsFlags);
    ImGui::Image((void*)(U64)mDeferredRoughness->GetID(), ImGui::GetWindowSize());
    ImGui::End();
  }

private:

  void GeometryPass()
  {
    OPTICK_FRAME("GeometryPass");
    // Dequeue render tasks and fill draw calls
    while (!mRenderQueue.empty())
    {
      RenderTask task = mRenderQueue.front();
      mDrawCalls[task.GetProgram()].emplace(task);
      mRenderQueue.pop();
    }
    // Dispatch all draw calls
    for (auto const& [program, tasks] : mDrawCalls)
    {
      program->Bind();
      for (auto const& task : tasks)
      {
        mProjection.Model = task.GetTransform()->GetMatrix();
        mUniformProjection->Set(&mProjection);
        task.GetMesh()->Bind();
        task.GetMesh()->Draw();
      }
    }
    // Cleanup drawcalls
    mDrawCalls.clear();
  }

private:

  UniformBuffer<Projection>* mUniformProjection;
  Texture2DU24U8DS* mDeferredDepthStencil;
  Texture2DR32RGBA* mDeferredPosition;
  Texture2DR32RGBA* mDeferredAlbedo;
  Texture2DR32RGBA* mDeferredNormal;
  Texture2DR32RGBA* mDeferredSpecular;
  Texture2DR32RGBA* mDeferredHeight;
  Texture2DR32RGBA* mDeferredMetallic;
  Texture2DR32RGBA* mDeferredRoughness;
  FrameBuffer* mFrameBuffer;

  Projection mProjection = {};
};