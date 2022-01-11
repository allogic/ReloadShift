#pragma once

#include <Core.h>
#include <Component.h>
#include <HotRef.h>

#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>
#include <Handles/GenericTexture.h>

#include <Renderer/DeferredRenderTask.h>

class Transform;

class Renderable : public Component
{
public:

  Renderable(
    World& world,
    std::string const& meshName,
    std::string const& programName,
    std::string const& textureAlbedoName = "",
    std::string const& textureNormalName = "",
    std::string const& textureSpecularName = "",
    std::string const& textureMetallicName = "",
    std::string const& textureRoughnessName = "")
    : Component(world)
    , mMesh{ World::GetHandle<DefaultMesh>(mWorld, meshName) }
    , mProgram{ World::GetHandle<RenderProgram>(mWorld, programName) }
    , mTextureAlbedo{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureAlbedoName) }
    , mTextureNormal{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureNormalName) }
    , mTextureSpecular{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureSpecularName) }
    , mTextureMetallic{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureMetallicName) }
    , mTextureRoughness{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureRoughnessName) }
  {

  }

public:

  void SubmitRenderTask(Transform* transform, std::queue<DeferredRenderTask>& renderQueue)
  {
    // Minimum requirements are mesh and program
    if (mMesh.Get() && mProgram.Get())
    {
      renderQueue.emplace(DeferredRenderTask
        {
          transform,
          mMesh.Get(),
          mProgram.Get(),
          mTextureAlbedo.Get(),
          mTextureNormal.Get(),
          mTextureSpecular.Get(),
          mTextureMetallic.Get(),
          mTextureRoughness.Get()
        });
    }
  }

public:

  HotRef<DefaultMesh> const& mMesh;
  HotRef<RenderProgram> const& mProgram;
  HotRef<Texture2DR32RGBA> const& mTextureAlbedo;
  HotRef<Texture2DR32RGBA> const& mTextureNormal;
  HotRef<Texture2DR32RGBA> const& mTextureSpecular;
  HotRef<Texture2DR32RGBA> const& mTextureMetallic;
  HotRef<Texture2DR32RGBA> const& mTextureRoughness;
};