#pragma once

#include <Core.h>
#include <Resource.h>

class TextureResource : public Resource
{
public:

  TextureResource(
    World* world,
    std::string const& name,
    std::filesystem::path const& filePath);

public:

  virtual inline std::string GetType() const override { return "Texture"; }

public:

  virtual bool LoadFile() override;
  virtual bool ProduceHandles() override;

private:

  U32 mWidth = 0;
  U32 mHeight = 0;
  U32 mChannels = 0;

  U32 mGLType = 0;
  U32 mGLFormat = 0;
  U32 mGLFormatInternal = 0;
};