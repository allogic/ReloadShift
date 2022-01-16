#pragma once

#include <Core.h>
#include <Handle.h>
#include <Utility.h>

#include <Handles/GenericTexture.h>

template<U32 Layout, U32 Format, U32 FormatInternal, typename ... ColorTextures>
class GenericFrameBuffer : public Handle
{
public:

  using DepthStencilTexture = GenericTexture<GL_TEXTURE_2D, Layout, Format, FormatInternal>;

public:

  GenericFrameBuffer(
    std::string const& name,
    DepthStencilTexture* depthStencilTexture,
    ColorTextures* ... colorTextures)
    : Handle(name)
    , mDepthStencilTexture{ depthStencilTexture }
    , mColorTextures{ colorTextures ... }
  {
    glGenFramebuffers(1, &mID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mID);
    U32 colorTextureIndex = 0;
    U32 bufferAttachmentIndex = 0;
    U32 bufferAttachments[sizeof ... (ColorTextures)];
    ((glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + bufferAttachmentIndex++, GL_TEXTURE_2D, ((typename TypeProxy<ColorTextures>::CPtr)mColorTextures[colorTextureIndex++])->GetID(), 0)), ...);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilTexture->GetID(), 0);
    for (U32 i = 0; i < (sizeof ... (ColorTextures)); ++i)
    {
      bufferAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(sizeof ... (ColorTextures), bufferAttachments);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }
  virtual ~GenericFrameBuffer()
  {
    glDeleteFramebuffers(1, &mID);
  }

public:

  virtual inline std::string GetType() const override { return "FrameBuffer"; }

public:

  inline U32 GetID() const { return mID; }

public:

  void BindRead() const
  {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mID);
  }
  void BindWrite() const
  {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mID);
  }
  void UnBind() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

private:

  DepthStencilTexture* mDepthStencilTexture;
  void* mColorTextures[sizeof ... (ColorTextures)];

  U32 mID = 0;
};

template<typename ... ColorTexture>
using FrameBufferU24U8DS = GenericFrameBuffer<GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, ColorTexture ...>;