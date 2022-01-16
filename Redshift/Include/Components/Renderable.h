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

  void SubmitRenderTask(Transform* transform, std::queue<DeferredRenderTask>& renderQueue);

public:

  HotRef<DefaultMesh> const& mMesh;
  HotRef<RenderProgram> const& mProgram;
  HotRef<Texture2DR32RGBA> const& mTextureAlbedo;
  HotRef<Texture2DR32RGBA> const& mTextureNormal;
  HotRef<Texture2DR32RGBA> const& mTextureSpecular;
  HotRef<Texture2DR32RGBA> const& mTextureMetallic;
  HotRef<Texture2DR32RGBA> const& mTextureRoughness;
  HotRef<Texture2DR32RGBA> const& mTextureAmbientOcclusion;
};