#pragma once

#include <Core.h>
#include <Handle.h>

template<U32 Type, U32 Layout, U32 Format, U32 FormatInternal>
class GenericTexture : public Handle
{
public:

  GenericTexture(
    std::string const& name,
    U32 width,
    U32 height,
    ETextureWrap::Type wrap,
    ETextureFilter::Type filter)
    : Handle(typeid(GenericTexture).name(), name)
    , mWidth{ width }
    , mHeight{ height }
    , mWrap{ wrap }
    , mFilter{ filter }
  {

  }

public:

  virtual void Create() override
  {
    glGenTextures(1, &mID);
    glBindTexture(Type, mID);
    glTextureParameteri(mID, GL_TEXTURE_WRAP_S, (I32)mWrap);
    glTextureParameteri(mID, GL_TEXTURE_WRAP_T, (I32)mWrap);
    glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
    glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, (I32)mFilter);
    glTexImage2D(Type, 0, FormatInternal, mWidth, mHeight, 0, Format, Layout, nullptr);
    glBindTexture(Type, 0);
  }
  virtual void Destroy() override
  {
    glDeleteTextures(1, &mID);
  }

public:

  void Bind()
  {
    glBindTexture(Type, mID);
  }
  void Mount(U32 index)
  {
    glBindTextureUnit(index, mID);
  }
  void UnBind()
  {
    glBindTexture(Type, 0);
  }

private:

  U32 mWidth;
  U32 mHeight;
  ETextureWrap::Type mWrap;
  ETextureFilter::Type mFilter;

  U32 mID;
};

using Texture2DU8RGB = GenericTexture<GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB8UI>;
using Texture2DU8RGBA = GenericTexture<GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8UI>;

using Texture2DR32RGB = GenericTexture<GL_TEXTURE_2D, GL_FLOAT, GL_RGB, GL_RGB32F>;
using Texture2DR32RGBA = GenericTexture<GL_TEXTURE_2D, GL_FLOAT, GL_RGBA, GL_RGBA32F>;