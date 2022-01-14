#pragma once

#include <Core.h>
#include <Renderer.h>

#include <Components/Renderable.h>

#include <Renderer/DeferredRenderTask.h>

#include <Uniforms/UniformCamera.h>
#include <Uniforms/UniformModel.h>

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
    , mUniformCameraBuffer{ World::LinkHandle<UniformBuffer<CameraUniform>>(mWorld, "CameraUniform", 1u) }
    , mUniformModelBuffer{ World::LinkHandle<UniformBuffer<ModelUniform>>(mWorld, "ModelUniform", 1u) }
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
          transform->Update();
          renderable->SubmitRenderTask(transform, mRenderQueue);
        });
    // Update projection uniform
    mUniformCameraBuffer.Get()->Bind();
    World::DispatchFor<
      Transform,
      Camera>(mWorld, [&](Transform* transform, Camera* camera)
        {
          transform->Update();
          mUniformCamera.Projection = glm::perspective(glm::radians(camera->GetFOV()), mViewportSize.x / mViewportSize.y, camera->GetNear(), camera->GetFar());
          mUniformCamera.View = glm::lookAt(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalForward(), transform->GetLocalUp());
          mUniformCameraBuffer.Get()->Set(&mUniformCamera);
        });
    mUniformCameraBuffer.Get()->UnBind();
    // Render geometry into framebuffer
    mFrameBuffer.Get()->BindWrite();
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (I32)mViewportSize.x, (I32)mViewportSize.y);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    GeometryPass();
    glDisable(GL_DEPTH_TEST);
    mFrameBuffer.Get()->UnBind();
    // Blend framebuffer depth
    //mFrameBuffer->BindRead();
    //glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    //mFrameBuffer->UnBind();
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
    // Bind model uniform
    mUniformModelBuffer.Get()->Bind();
    // Mount uniforms
    mUniformCameraBuffer.Get()->Mount(0);
    mUniformModelBuffer.Get()->Mount(1);
    // Dispatch all draw calls
    for (auto const& [program, tasks] : mDrawCalls)
    {
      program->Bind();
      for (auto const& task : tasks)
      {
        // Update model uniform
        mUniformModel.Model = task.GetTransform()->GetMatrix();
        mUniformModelBuffer.Get()->Set(&mUniformModel);
        // Mount textures
        if (Texture2DR32RGBA* texture = task.GetTextureAlbedo()) texture->Mount(0);
        if (Texture2DR32RGBA* texture = task.GetTextureNormal()) texture->Mount(1);
        if (Texture2DR32RGBA* texture = task.GetTextureSpecular()) texture->Mount(2);
        if (Texture2DR32RGBA* texture = task.GetTextureMetallic()) texture->Mount(3);
        if (Texture2DR32RGBA* texture = task.GetTextureRoughness()) texture->Mount(4);
        // Draw mesh
        task.GetMesh()->Bind();
        task.GetMesh()->Draw();
        task.GetMesh()->UnBind();
      }
      program->UnBind();
    }
    // Cleanup drawcalls
    mDrawCalls.clear();
    // UnBind model uniform
    mUniformModelBuffer.Get()->UnBind();
  }

private:

  HotRef<UniformBuffer<CameraUniform>> const& mUniformCameraBuffer;
  HotRef<UniformBuffer<ModelUniform>> const& mUniformModelBuffer;

  HotRef<Texture2DU24U8DS> const& mTextureDepthStencil;
  HotRef<Texture2DR32RGBA> const& mTexturePosition;
  HotRef<Texture2DR32RGBA> const& mTextureAlbedo;
  HotRef<Texture2DR32RGBA> const& mTextureNormal;
  HotRef<Texture2DR32RGBA> const& mTextureSpecular;
  HotRef<Texture2DR32RGBA> const& mTextureMetallic;
  HotRef<Texture2DR32RGBA> const& mTextureRoughness;

  HotRef<DeferredFrameBuffer> const& mFrameBuffer;

  CameraUniform mUniformCamera = {};
  ModelUniform mUniformModel = {};

  std::queue<DeferredRenderTask> mRenderQueue = {};
  std::unordered_map<RenderProgram*, std::multiset<DeferredRenderTask>> mDrawCalls = {};
};