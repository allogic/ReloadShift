#pragma once

#include <Redshift.h>

#include <Renderer/DeferredRenderTask.h>

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

  DeferredRenderer(World* world)
    : Renderer(world)
    , mUniformProjection{ mWorld->LinkHandle<UniformBuffer<Projection>>("Projection", 1u) }
    , mTextureDepthStencil{ mWorld->LinkHandle<Texture2DU24U8DS>("DeferredDepthStencil", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTexturePosition{ mWorld->LinkHandle<Texture2DR32RGBA>("DeferredPosition", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureAlbedo{ mWorld->LinkHandle<Texture2DR32RGBA>("DeferredAlbedo", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureNormal{ mWorld->LinkHandle<Texture2DR32RGBA>("DeferredNormal", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureSpecular{ mWorld->LinkHandle<Texture2DR32RGBA>("DeferredSpecular", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureMetallic{ mWorld->LinkHandle<Texture2DR32RGBA>("DeferredMetallic", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mTextureRoughness{ mWorld->LinkHandle<Texture2DR32RGBA>("DeferredRoughness", 1280u, 720u, ETextureWrap::ClampToEdge, ETextureFilter::Nearest) }
    , mFrameBuffer{ mWorld->LinkHandle<DeferredFrameBuffer>("DeferredFrameBuffer", mTextureDepthStencil.Get(), mTexturePosition.Get(), mTextureAlbedo.Get(), mTextureNormal.Get(), mTextureSpecular.Get(), mTextureMetallic.Get(), mTextureRoughness.Get()) }
    , mCurrentTextureID{ mTexturePosition.Get()->GetID() }
  {
    
  }

public:

  virtual void Render() override
  {
    OPTICK_FRAME("DeferredRender");
    // Submit render tasks for all renderables
    mWorld->DispatchFor<
      Transform,
      Renderable>(mWorld, [=](Transform* transform, Renderable* renderable)
        {
          renderable->SubmitRenderTask(transform, mRenderQueue);
        });
    // Bind uniforms
    mUniformProjection.Get()->Bind();
    mUniformProjection.Get()->Mount(0);
    // Update projection uniform
    mWorld->DispatchFor<
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

public:

  void Debug()
  {
    static bool opened = true;
    R32V2 currentSize = R32V2{};
    bool resize = false;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    ImGui::Begin("Viewport", &opened, windowFlags);
    ImGui::PopStyleVar();
    ImGui::BeginMenuBar();
    if (ImGui::BeginMenu("Buffer"))
    {
      if (ImGui::MenuItem("DepthStencil")) mCurrentTextureID = mTextureDepthStencil.Get()->GetID();
      if (ImGui::MenuItem("Position")) mCurrentTextureID = mTexturePosition.Get()->GetID();
      if (ImGui::MenuItem("Albedo")) mCurrentTextureID = mTextureAlbedo.Get()->GetID();
      if (ImGui::MenuItem("Normal")) mCurrentTextureID = mTextureNormal.Get()->GetID();
      if (ImGui::MenuItem("Specular")) mCurrentTextureID = mTextureSpecular.Get()->GetID();
      if (ImGui::MenuItem("Metallic")) mCurrentTextureID = mTextureMetallic.Get()->GetID();
      if (ImGui::MenuItem("Roughness")) mCurrentTextureID = mTextureRoughness.Get()->GetID();
      ImGui::EndMenu();
    }
    currentSize = R32V2{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };
    ImGui::Text("%ux%u", (U32)mViewportSize.x, (U32)mViewportSize.y);
    ImGui::EndMenuBar();
    if (mViewportSize != currentSize)
    {
      mViewportSize = currentSize;
      if (Texture2DU24U8DS* texture = mTextureDepthStencil.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
      if (Texture2DR32RGBA* texture = mTexturePosition.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
      if (Texture2DR32RGBA* texture = mTextureAlbedo.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
      if (Texture2DR32RGBA* texture = mTextureNormal.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
      if (Texture2DR32RGBA* texture = mTextureSpecular.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
      if (Texture2DR32RGBA* texture = mTextureMetallic.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
      if (Texture2DR32RGBA* texture = mTextureRoughness.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    }
    ImGui::Image((void*)(U64)mCurrentTextureID, ImVec2{ mViewportSize.x , mViewportSize.y - 39.0f });
    ImGui::End();
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
        mProjection.Model = task.GetTransform()->GetMatrix();
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

  R32V2 mViewportSize = R32V2{};
  U32 mCurrentTextureID = 0;

  Projection mProjection = {};

  std::queue<DeferredRenderTask> mRenderQueue = {};
  std::unordered_map<RenderProgram*, std::multiset<DeferredRenderTask>> mDrawCalls = {};
};