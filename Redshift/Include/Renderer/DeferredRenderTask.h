#pragma once

#include <Core.h>

#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>
#include <Handles/GenericTexture.h>

class Transform;

class DeferredRenderTask
{
public:

  DeferredRenderTask(
    Transform* transform,
    DefaultMesh* mesh,
    RenderProgram* program,
    Texture2DR32RGBA* textureAlbedo,
    Texture2DR32RGBA* textureNormal,
    Texture2DR32RGBA* textureSpecular,
    Texture2DR32RGBA* textureMetallic,
    Texture2DR32RGBA* textureRoughness,
    Texture2DR32RGBA* textureAmbientOcclusion)
    : mTransform{ transform }
    , mMesh{ mesh }
    , mProgram{ program }
    , mTextureAlbedo{ textureAlbedo }
    , mTextureNormal{ textureNormal }
    , mTextureSpecular{ textureSpecular }
    , mTextureMetallic{ textureMetallic }
    , mTextureRoughness{ textureRoughness }
    , mTextureAmbientOcclusion{ textureAmbientOcclusion }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline DefaultMesh* GetMesh() const { return mMesh; }
  inline RenderProgram* GetProgram() const { return mProgram; }
  inline Texture2DR32RGBA* GetTextureAlbedo() const { return mTextureAlbedo; }
  inline Texture2DR32RGBA* GetTextureNormal() const { return mTextureNormal; }
  inline Texture2DR32RGBA* GetTextureSpecular() const { return mTextureSpecular; }
  inline Texture2DR32RGBA* GetTextureMetallic() const { return mTextureMetallic; }
  inline Texture2DR32RGBA* GetTextureRoughness() const { return mTextureRoughness; }
  inline Texture2DR32RGBA* GetTextureAmbientOcclusion() const { return mTextureAmbientOcclusion; }

public:

  bool operator < (DeferredRenderTask const& other) const { return mProgram < other.mProgram; }

private:

  Transform* mTransform;
  DefaultMesh* mMesh;
  RenderProgram* mProgram;
  Texture2DR32RGBA* mTextureAlbedo;
  Texture2DR32RGBA* mTextureNormal;
  Texture2DR32RGBA* mTextureSpecular;
  Texture2DR32RGBA* mTextureMetallic;
  Texture2DR32RGBA* mTextureRoughness;
  Texture2DR32RGBA* mTextureAmbientOcclusion;
};