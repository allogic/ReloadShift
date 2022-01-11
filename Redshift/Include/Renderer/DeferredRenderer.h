#pragma once

#include <Core.h>
#include <Renderer.h>

#include <Components/Renderable.h>

#include <Renderer/DeferredRenderTask.h>

#include <Uniforms/Projection.h>

class DeferredRenderer : public Renderer
{
public:

  using DeferredFrameBuffer = FrameBufferU24U8DS<
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA,
    Texture2DR32RGBA>;

public:

  DeferredRenderer(std::string const& name)
    : Renderer(name)
    , mUniformProjection{ World::LinkHandle<UniformBuffer<Projection>>(mWorld, "Projection", 1u) }
    , mTextureDepthStencil{ World::LinkHandle<Texture2DU24U8DS>(mWorld, "DeferredDepthStencil", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTexturePosition{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredPosition", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureAlbedo{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredAlbedo", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureNormal{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredNormal", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureSpecular{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredSpecular", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureMetallic{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredMetallic", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureRoughness{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredRoughness", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mFrameBuffer{ World::LinkHandle<DeferredFrameBuffer>(mWorld, "DeferredFrameBuffer", mTextureDepthStencil.Get(), mTexturePosition.Get(), mTextureAlbedo.Get(), mTextureNormal.Get(), mTextureSpecular.Get(), mTextureMetallic.Get(), mTextureRoughness.Get()) }
  {
    // Mapping required for UI
    mBuffers[mTextureDepthStencil.Get()->GetName()] = mTextureDepthStencil.Get()->GetID();
    mBuffers[mTexturePosition.Get()->GetName()] = mTexturePosition.Get()->GetID();
    mBuffers[mTextureAlbedo.Get()->GetName()] = mTextureAlbedo.Get()->GetID();
    mBuffers[mTextureNormal.Get()->GetName()] = mTextureNormal.Get()->GetID();
    mBuffers[mTextureSpecular.Get()->GetName()] = mTextureSpecular.Get()->GetID();
    mBuffers[mTextureMetallic.Get()->GetName()] = mTextureMetallic.Get()->GetID();
    mBuffers[mTextureRoughness.Get()->GetName()] = mTextureRoughness.Get()->GetID();
  }

public:

  virtual void Render() override
  {
    OPTICK_FRAME("DeferredRender");
    // Submit render tasks for all renderables
    World::DispatchFor<
      Transform,
      Renderable>(mWorld, [=](Transform* transform, Renderable* renderable)
        {
          renderable->SubmitRenderTask(transform, mRenderQueue);
        });
    // Bind uniforms
    mUniformProjection.Get()->Bind();
    mUniformProjection.Get()->Mount(0);
    // Update projection uniform
    World::DispatchFor<
      Transform,
      Camera>(mWorld, [&](Transform* transform, Camera* camera)
        {
          mProjection.Projection = glm::perspective(glm::radians(camera->GetFOV()), mViewportSize.x / mViewportSize.y, camera->GetNear(), camera->GetFar());
          mProjection.View = glm::lookAt(transform->GetWorldPosition(), transform->GetWorldPosition() + R32V3{ 0.0f, 0.0f, 1.0f }, R32V3{ 0.0f, 1.0f, 0.0f });
        });
    // Render geometry into framebuffer
    mFrameBuffer.Get()->BindWrite();
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (I32)mViewportSize.x, (I32)mViewportSize.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    GeometryPass();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    mFrameBuffer.Get()->UnBind();
    // Blend framebuffer depth
    //mFrameBuffer->BindRead();
    //glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    //mFrameBuffer->UnBind();
    // UnBind uniforms
    mUniformProjection.Get()->UnBind();
  }
  virtual void ResizeViewport() override
  {
    if (Texture2DU24U8DS* texture = mTextureDepthStencil.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTexturePosition.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureAlbedo.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureNormal.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureSpecular.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureMetallic.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureRoughness.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
  }

private:

  void GeometryPass()
  {
    OPTICK_FRAME("GeometryPass");
    // Dequeue render tasks and fill draw calls
    while (!mRenderQueue.empty())
    {
      DeferredRenderTask task = mRenderQueue.front();
      mDrawCalls[task.GetProgram()].emplace(task);
      mRenderQueue.pop();
    }
    // Dispatch all draw calls
    for (auto const& [program, tasks] : mDrawCalls)
    {
      program->Bind();
      for (auto const& task : tasks)
      {
        // Update projection
        mProjection.Model = task.GetTransform()->GetModelMatrix();
        mUniformProjection.Get()->Set(&mProjection);
        // Bind textures
        if (task.GetTextureAlbedo()) { task.GetTextureAlbedo()->Bind(); task.GetTextureAlbedo()->Mount(0); }
        if (task.GetTextureNormal()) { task.GetTextureNormal()->Bind(); task.GetTextureNormal()->Mount(1); }
        if (task.GetTextureSpecular()) { task.GetTextureSpecular()->Bind(); task.GetTextureSpecular()->Mount(2); }
        if (task.GetTextureMetallic()) { task.GetTextureMetallic()->Bind(); task.GetTextureMetallic()->Mount(3); }
        if (task.GetTextureRoughness()) { task.GetTextureRoughness()->Bind(); task.GetTextureRoughness()->Mount(4); }
        // Draw mesh
        task.GetMesh()->Bind();
        task.GetMesh()->Draw();
        task.GetMesh()->UnBind();
        // UnBind textures
        if (task.GetTextureAlbedo()) task.GetTextureAlbedo()->UnBind();
        if (task.GetTextureNormal()) task.GetTextureNormal()->UnBind();
        if (task.GetTextureSpecular()) task.GetTextureSpecular()->UnBind();
        if (task.GetTextureMetallic()) task.GetTextureMetallic()->UnBind();
        if (task.GetTextureRoughness()) task.GetTextureRoughness()->UnBind();
      }
      program->UnBind();
    }
    // Cleanup drawcalls
    mDrawCalls.clear();
  }

private:

  HotRef<UniformBuffer<Projection>> const& mUniformProjection;

  HotRef<Texture2DU24U8DS> const& mTextureDepthStencil;
  HotRef<Texture2DR32RGBA> const& mTexturePosition;
  HotRef<Texture2DR32RGBA> const& mTextureAlbedo;
  HotRef<Texture2DR32RGBA> const& mTextureNormal;
  HotRef<Texture2DR32RGBA> const& mTextureSpecular;
  HotRef<Texture2DR32RGBA> const& mTextureMetallic;
  HotRef<Texture2DR32RGBA> const& mTextureRoughness;

  HotRef<DeferredFrameBuffer> const& mFrameBuffer;

  Projection mProjection = {};

  std::queue<DeferredRenderTask> mRenderQueue = {};
  std::unordered_map<RenderProgram*, std::multiset<DeferredRenderTask>> mDrawCalls = {};
};