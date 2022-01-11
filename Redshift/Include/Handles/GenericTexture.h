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
    ETextureFilter::Type filter,
    void* data = nullptr)
    : Handle(name)
    , mWidth{ width }
    , mHeight{ height }
    , mWrap{ wrap }
    , mFilter{ filter }
  {
    glGenTextures(1, &mID);
    glBindTexture(Type, mID);
    glTextureParameteri(mID, GL_TEXTURE_WRAP_S, (I32)mWrap);
    glTextureParameteri(mID, GL_TEXTURE_WRAP_T, (I32)mWrap);
    glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, (I32)mFilter);
    glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, (I32)mFilter);
    glTexImage2D(Type, 0, FormatInternal, mWidth, mHeight, 0, Format, Layout, data);
    glBindTexture(Type, 0);
  }
  virtual ~GenericTexture()
  {
    glDeleteTextures(1, &mID);
  }

public:

  virtual inline std::string GetType() const override { return "Texture"; }

public:

  inline U32 GetID() const { return mID; }

public:

  void Bind() const
  {
    glBindTexture(Type, mID);
  }
  void Resize(R32V2 size) const
  {
    glTexImage2D(Type, 0, FormatInternal, (I32)size.x, (I32)size.y, 0, Format, Layout, nullptr);
  }
  void Mount(U32 index) const
  {
    glBindTextureUnit(index, mID);
  }
  void UnBind() const
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

using Texture2DU24U8DS = GenericTexture<GL_TEXTURE_2D, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8>;