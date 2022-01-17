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
    std::string const& textureRoughnessName = "",
    std::string const& textureAmbientOcclusionName = "");

public:

  inline DefaultMesh const* GetMesh() const { return mMesh.Get(); }
  inline RenderProgram const* GetProgram() const { return mProgram.Get(); }
  inline Texture2DR32RGBA const* GetTextureAlbedo() const { return mTextureAlbedo.Get(); }
  inline Texture2DR32RGBA const* GetTextureNormal() const { return mTextureNormal.Get(); }
  inline Texture2DR32RGBA const* GetTextureSpecular() const { return mTextureSpecular.Get(); }
  inline Texture2DR32RGBA const* GetTextureMetallic() const { return mTextureMetallic.Get(); }
  inline Texture2DR32RGBA const* GetTextureRoughness() const { return mTextureRoughness.Get(); }
  inline Texture2DR32RGBA const* GetTextureAmbientOcclusion() const { return mTextureAmbientOcclusion.Get(); }

public:

  void SubmitRenderTask(Transform* transform, std::queue<DeferredRenderTask>& renderQueue) const;

private:

  HotRef<DefaultMesh> const& mMesh;
  HotRef<RenderProgram> const& mProgram;
  HotRef<Texture2DR32RGBA> const& mTextureAlbedo;
  HotRef<Texture2DR32RGBA> const& mTextureNormal;
  HotRef<Texture2DR32RGBA> const& mTextureSpecular;
  HotRef<Texture2DR32RGBA> const& mTextureMetallic;
  HotRef<Texture2DR32RGBA> const& mTextureRoughness;
  HotRef<Texture2DR32RGBA> const& mTextureAmbientOcclusion;
};