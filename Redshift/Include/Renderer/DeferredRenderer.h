#pragma once

#include <Core.h>
#include <Renderer.h>

#include <Components/Renderable.h>

#include <Renderer/DeferredRenderTask.h>

#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>
#include <Uniforms/ModelUniform.h>

class DeferredRenderer : public Renderer
{
public:

  ////////////////////////////////////////////////////////
  // Note: OpenGL generates errors when attaching more
  //       than 6 or 7 color attachments.
  //       Graphics card: GTX 3080
  //       Driver version: 497.09
  ////////////////////////////////////////////////////////

  using DeferredFrameBuffer = FrameBufferU24U8DS<
    Texture2DR32RGBA,  // Lit
    Texture2DR32RGBA,  // Unlit
    Texture2DR32RGBA,  // Normal
    Texture2DR32RGBA,  // Specular
    Texture2DR32RGBA,  // Metallic
    Texture2DR32RGBA>; // Roughness
    //Texture2DR32RGBA,  // AmbientOcclusion
    //Texture2DR32RGBA,  // WorldTangent
    //Texture2DR32RGBA,  // WorldNormal
    //Texture2DR32RGBA>; // Depth

public:

  DeferredRenderer(
    World& world,
    std::string const& name)
    : Renderer(world, name)
    , mUniformBufferCamera{ World::LinkHandle<UniformBuffer<UniformCamera>>(mWorld, "UniformBufferCamera", 1u) }
    , mUniformBufferModel{ World::LinkHandle<UniformBuffer<UniformModel>>(mWorld, "UniformBufferModel", 1u) }
    , mUniformBufferAmbientLight{ World::LinkHandle<UniformBuffer<UniformAmbientLight>>(mWorld, "UniformBufferAmbientLight", 32u) }
    , mUniformBufferDirectionLight{ World::LinkHandle<UniformBuffer<UniformDirectionLight>>(mWorld, "UniformBufferDirectionLight", 32u) }
    , mUniformBufferPointLight{ World::LinkHandle<UniformBuffer<UniformPointLight>>(mWorld, "UniformBufferPointLight", 32u) }
    , mUniformBufferSpotLight{ World::LinkHandle<UniformBuffer<UniformSpotLight>>(mWorld, "UniformBufferSpotLight", 32u) }
    , mTextureDepthStencil{ World::LinkHandle<Texture2DU24U8DS>(mWorld, "DeferredDepthStencil") }
    , mTextureLit{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredLit") }
    , mTextureUnlit{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredUnlit") }
    , mTextureNormal{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredNormal") }
    , mTextureSpecular{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredSpecular") }
    , mTextureMetallic{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredMetallic") }
    , mTextureRoughness{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredRoughness") }
    , mTextureAmbientOcclusion{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredAmbientOcclusion") }
    , mTextureWorldTangent{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredWorldTangent") }
    , mTextureWorldNormal{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredWorldNormal") }
    , mTextureDepth{ World::LinkHandle<Texture2DR32RGBA>(mWorld, "DeferredDepth") }
    , mFrameBuffer{ World::LinkHandle<DeferredFrameBuffer>(mWorld, "DeferredFrameBuffer", mTextureDepthStencil.Get(), mTextureLit.Get(), mTextureUnlit.Get(), mTextureNormal.Get(), mTextureSpecular.Get(), mTextureMetallic.Get(), mTextureRoughness.Get()) }
  {
    // Mapping required for UI
    mBuffers[mTextureDepthStencil.Get()->GetName()] = mTextureDepthStencil.Get()->GetID();
    mBuffers[mTextureLit.Get()->GetName()] = mTextureLit.Get()->GetID();
    mBuffers[mTextureUnlit.Get()->GetName()] = mTextureUnlit.Get()->GetID();
    mBuffers[mTextureNormal.Get()->GetName()] = mTextureNormal.Get()->GetID();
    mBuffers[mTextureSpecular.Get()->GetName()] = mTextureSpecular.Get()->GetID();
    mBuffers[mTextureMetallic.Get()->GetName()] = mTextureMetallic.Get()->GetID();
    mBuffers[mTextureRoughness.Get()->GetName()] = mTextureRoughness.Get()->GetID();
    mBuffers[mTextureAmbientOcclusion.Get()->GetName()] = mTextureAmbientOcclusion.Get()->GetID();
    mBuffers[mTextureWorldTangent.Get()->GetName()] = mTextureWorldTangent.Get()->GetID();
    mBuffers[mTextureWorldNormal.Get()->GetName()] = mTextureWorldNormal.Get()->GetID();
    mBuffers[mTextureDepth.Get()->GetName()] = mTextureDepth.Get()->GetID();
  }

public:

  virtual void Render() override
  {
    // Prepare for new frame
    SubmitRenderTasks();
    UpdateCameraUniforms();
    UpdateLightUniforms();
    // Render geometry into framebuffer
    mFrameBuffer.Get()->BindWrite();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, (I32)mViewportSize.x, (I32)mViewportSize.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    GeometryPass();
    glDisable(GL_CULL_FACE);
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
    if (Texture2DR32RGBA* texture = mTextureLit.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureUnlit.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureNormal.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureSpecular.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureMetallic.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureRoughness.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureAmbientOcclusion.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureWorldTangent.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureWorldNormal.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
    if (Texture2DR32RGBA* texture = mTextureDepth.Get()) { texture->Bind(); texture->Resize(mViewportSize); texture->UnBind(); }
  }

private:

  void SubmitRenderTasks()
  {
    World::DispatchFor<
      Transform,
      Renderable>(mWorld, [=](Transform* transform, Renderable* renderable)
        {
          transform->Update();
          renderable->SubmitRenderTask(transform, mRenderQueue);
        });
  }
  void UpdateCameraUniforms()
  {
    mUniformBufferCamera.Get()->Bind();
    World::DispatchFor<
      Transform,
      Camera>(mWorld, [=](Transform* transform, Camera* camera)
        {
          transform->Update();
          UniformCamera uniformCamera
          {
            .Projection = glm::perspective(glm::radians(camera->GetFOV()), mViewportSize.x / mViewportSize.y, camera->GetNear(), camera->GetFar()),
            .View = glm::lookAt(transform->GetWorldPosition(), transform->GetWorldPosition() + transform->GetLocalForward(), transform->GetLocalUp()),
          };
          mUniformBufferCamera.Get()->Set(&uniformCamera);
        });
    mUniformBufferCamera.Get()->UnBind();
  }
  void UpdateLightUniforms()
  {
    // Update ambient lights
    U32 ambientLightIndex = 0;
    mUniformBufferAmbientLight.Get()->Bind();
    World::DispatchFor<
      AmbientLight>(mWorld, [&](AmbientLight* ambientLight)
        {
          UniformAmbientLight uniformAmbientLight
          {
            .Intensity = ambientLight->GetIntensity(),
            .Color = R32V4{ ambientLight->GetColor(), 0.0f },
            .Enabled = ambientLight->GetEnabled(),
          };
          mUniformBufferAmbientLight.Get()->Set(ambientLightIndex++, &uniformAmbientLight, 1);
        });
    mUniformBufferAmbientLight.Get()->UnBind();
    // Update direction lights
    U32 directionLightIndex = 0;
    mUniformBufferDirectionLight.Get()->Bind();
    World::DispatchFor<
      Transform,
      DirectionLight>(mWorld, [&](Transform* transform, DirectionLight* directionLight)
        {
          UniformDirectionLight uniformDirectionLight
          {
            .Intensity = directionLight->GetIntensity(),
            .Color = R32V4{ directionLight->GetColor(), 0.0f },
            .Direction = R32V4{ transform->GetWorldRotation(), 0.0f },
            .Enabled = directionLight->GetEnabled(),
          };
          mUniformBufferDirectionLight.Get()->Set(directionLightIndex++, &uniformDirectionLight, 1);
        });
    mUniformBufferDirectionLight.Get()->UnBind();
    // Update point lights
    U32 pointLightIndex = 0;
    mUniformBufferPointLight.Get()->Bind();
    World::DispatchFor<
      Transform,
      PointLight>(mWorld, [&](Transform* transform, PointLight* pointLight)
        {
          UniformPointLight uniformPointLight
          {
            .Intensity = pointLight->GetIntensity(),
            .Color = R32V4{ pointLight->GetColor(), 0.0f },
            .Position = R32V4{ transform->GetWorldPosition(), 0.0f },
            .Enabled = pointLight->GetEnabled(),
          };
          mUniformBufferPointLight.Get()->Set(pointLightIndex++, &uniformPointLight, 1);
        });
    mUniformBufferPointLight.Get()->UnBind();
    // Update spot lights
    U32 spotLightIndex = 0;
    mUniformBufferSpotLight.Get()->Bind();
    World::DispatchFor<
      Transform,
      SpotLight>(mWorld, [&](Transform* transform, SpotLight* spotLight)
        {
          UniformSpotLight uniformSpotLight
          {
            .Intensity = spotLight->GetIntensity(),
            .CutOff = spotLight->GetCutOff(),
            .Color = R32V4{ spotLight->GetColor(), 0.0f },
            .Position = R32V4{ transform->GetWorldPosition(), 0.0f },
            .Direction = R32V4{ transform->GetWorldRotation(), 0.0f },
            .Enabled = spotLight->GetEnabled(),
          };
          mUniformBufferSpotLight.Get()->Set(spotLightIndex++, &uniformSpotLight, 1);
        });
    mUniformBufferSpotLight.Get()->UnBind();
  }
  void GeometryPass()
  {
    // Dequeue render tasks and fill draw calls
    while (!mRenderQueue.empty())
    {
      DeferredRenderTask task = mRenderQueue.front();
      mDrawCalls[task.GetProgram()].emplace(task);
      mRenderQueue.pop();
    }
    // Bind and mount uniform buffers
    mUniformBufferCamera.Get()->Bind();
    mUniformBufferCamera.Get()->Mount(0);
    mUniformBufferAmbientLight.Get()->Bind();
    mUniformBufferAmbientLight.Get()->Mount(2);
    mUniformBufferDirectionLight.Get()->Bind();
    mUniformBufferDirectionLight.Get()->Mount(3);
    mUniformBufferPointLight.Get()->Bind();
    mUniformBufferPointLight.Get()->Mount(4);
    mUniformBufferSpotLight.Get()->Bind();
    mUniformBufferSpotLight.Get()->Mount(5);
    mUniformBufferModel.Get()->Bind();
    mUniformBufferModel.Get()->Mount(1);
    // Dispatch all draw calls
    for (auto const& [program, tasks] : mDrawCalls)
    {
      program->Bind();
      for (auto const& task : tasks)
      {
        // Update model uniform
        UniformModel uniformModel
        {
          .Model = task.GetTransform()->GetMatrix(),
        };
        mUniformBufferModel.Get()->Set(&uniformModel);
        // Bind and mount textures
        if (Texture2DR32RGBA* texture = task.GetTextureAlbedo()) texture->Mount(0);
        if (Texture2DR32RGBA* texture = task.GetTextureNormal()) texture->Mount(1);
        if (Texture2DR32RGBA* texture = task.GetTextureSpecular()) texture->Mount(2);
        if (Texture2DR32RGBA* texture = task.GetTextureMetallic()) texture->Mount(3);
        if (Texture2DR32RGBA* texture = task.GetTextureRoughness()) texture->Mount(4);
        if (Texture2DR32RGBA* texture = task.GetTextureAmbientOcclusion()) texture->Mount(5);
        // Draw mesh
        task.GetMesh()->Bind();
        task.GetMesh()->Draw();
        task.GetMesh()->UnBind();
        // UnBind texture
        glBindTexture(GL_TEXTURE_2D, 0);
      }
      program->UnBind();
    }
    // Cleanup drawcalls
    mDrawCalls.clear();
    // UnBind uniforms
    mUniformBufferModel.Get()->UnBind();
    mUniformBufferSpotLight.Get()->UnBind();
    mUniformBufferPointLight.Get()->UnBind();
    mUniformBufferDirectionLight.Get()->UnBind();
    mUniformBufferAmbientLight.Get()->UnBind();
  }

private:

  HotRef<UniformBuffer<UniformCamera>> const& mUniformBufferCamera;
  HotRef<UniformBuffer<UniformModel>> const& mUniformBufferModel;
  HotRef<UniformBuffer<UniformAmbientLight>> const& mUniformBufferAmbientLight;
  HotRef<UniformBuffer<UniformDirectionLight>> const& mUniformBufferDirectionLight;
  HotRef<UniformBuffer<UniformPointLight>> const& mUniformBufferPointLight;
  HotRef<UniformBuffer<UniformSpotLight>> const& mUniformBufferSpotLight;

  HotRef<Texture2DU24U8DS> const& mTextureDepthStencil;

  HotRef<Texture2DR32RGBA> const& mTextureLit;
  HotRef<Texture2DR32RGBA> const& mTextureUnlit;

  HotRef<Texture2DR32RGBA> const& mTextureNormal;
  HotRef<Texture2DR32RGBA> const& mTextureSpecular;
  HotRef<Texture2DR32RGBA> const& mTextureMetallic;
  HotRef<Texture2DR32RGBA> const& mTextureRoughness;
  HotRef<Texture2DR32RGBA> const& mTextureAmbientOcclusion;
  HotRef<Texture2DR32RGBA> const& mTextureWorldTangent;
  HotRef<Texture2DR32RGBA> const& mTextureWorldNormal;
  HotRef<Texture2DR32RGBA> const& mTextureDepth;

  HotRef<DeferredFrameBuffer> const& mFrameBuffer;

  std::queue<DeferredRenderTask> mRenderQueue = {};
  std::unordered_map<RenderProgram*, std::multiset<DeferredRenderTask>> mDrawCalls = {};
};