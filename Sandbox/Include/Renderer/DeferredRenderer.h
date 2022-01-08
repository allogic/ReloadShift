#pragma once

#include <Redshift.h>

class DeferredRenderer : public Renderer
{
public:

  using DeferredFrameBuffer = FrameBufferU24U8DS<
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
          renderable->SubmitRenderTask(transform, mRenderQueue);
        });
    // Bind uniforms
    mUniformProjection.Get()->Bind();
    mUniformProjection.Get()->Mount(0);
    // Update projection uniform
    mWorld->DispatchFor<
      Transform,
      Camera>([&](Transform* transform, Camera* camera)
        {
          mProjection.Projection = glm::perspective(glm::radians(camera->GetFOV()), 1280.0f / 720.0f, camera->GetNear(), camera->GetFar());
          mProjection.View = glm::lookAt(transform->GetPosition(), transform->GetPosition() + R32V3{ 0.0f, 0.0f, 1.0f }, R32V3{ 0.0f, 1.0f, 0.0f });
        });
    // Render geometry into framebuffer
    mFrameBuffer.Get()->BindWrite();
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GeometryPass();
    glDisable(GL_BLEND);
    mFrameBuffer.Get()->UnBind();
    // Blend framebuffer with background
    //mFrameBuffer->BindRead();
    //glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    //mFrameBuffer->UnBind();
    // UnBind uniforms
    mUniformProjection.Get()->UnBind();
  }

public:

  void Debug() const
  {
    ImGuiWindowFlags windowsFlags = ImGuiWindowFlags_NoDecoration;
    static bool opened = true;

    ImGui::Begin("DepthStencil", &opened);
    ImGui::Image((void*)(U64)mDeferredDepthStencil.Get()->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Position", &opened);
    ImGui::Image((void*)(U64)mDeferredPosition.Get()->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Albedo", &opened);
    ImGui::Image((void*)(U64)mDeferredAlbedo.Get()->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Normal", &opened);
    ImGui::Image((void*)(U64)mDeferredNormal.Get()->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Specular", &opened);
    ImGui::Image((void*)(U64)mDeferredSpecular.Get()->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Height", &opened);
    ImGui::Image((void*)(U64)mDeferredHeight.Get()->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Metallic", &opened);
    ImGui::Image((void*)(U64)mDeferredMetallic.Get()->GetID(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Begin("Roughness", &opened);
    ImGui::Image((void*)(U64)mDeferredRoughness.Get()->GetID(), ImGui::GetWindowSize());
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
        mUniformProjection.Get()->Set(&mProjection);
        task.GetMesh()->Bind();
        task.GetMesh()->Draw();
      }
    }
    // Cleanup drawcalls
    mDrawCalls.clear();
  }

private:

  HotRef<UniformBuffer<Projection>> const& mUniformProjection = mWorld->LinkHandle<UniformBuffer<Projection>>("Projection", 1u);
  HotRef<Texture2DU24U8DS> const& mDeferredDepthStencil = mWorld->LinkHandle<Texture2DU24U8DS>("DeferredDepthStencil", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<Texture2DR32RGBA> const& mDeferredPosition = mWorld->LinkHandle<Texture2DR32RGBA>("DeferredPosition", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<Texture2DR32RGBA> const& mDeferredAlbedo = mWorld->LinkHandle<Texture2DR32RGBA>("DeferredAlbedo", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<Texture2DR32RGBA> const& mDeferredNormal = mWorld->LinkHandle<Texture2DR32RGBA>("DeferredNormal", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<Texture2DR32RGBA> const& mDeferredSpecular = mWorld->LinkHandle<Texture2DR32RGBA>("DeferredSpecular", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<Texture2DR32RGBA> const& mDeferredHeight = mWorld->LinkHandle<Texture2DR32RGBA>("DeferredHeight", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<Texture2DR32RGBA> const& mDeferredMetallic = mWorld->LinkHandle<Texture2DR32RGBA>("DeferredMetallic", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<Texture2DR32RGBA> const& mDeferredRoughness = mWorld->LinkHandle<Texture2DR32RGBA>("DeferredRoughness", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
  HotRef<DeferredFrameBuffer> const& mFrameBuffer = mWorld->LinkHandle<DeferredFrameBuffer>("Deferred", 1280u, 720u, mDeferredDepthStencil.Get(), mDeferredPosition.Get(), mDeferredAlbedo.Get(), mDeferredNormal.Get(), mDeferredSpecular.Get(), mDeferredHeight.Get(), mDeferredMetallic.Get(), mDeferredRoughness.Get());

  Projection mProjection = {};
};