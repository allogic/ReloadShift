#pragma once

#include <Core.h>
#include <Handle.h>

#include <Handles/GenericTexture.h>

template<U32 Layout, U32 Format, U32 FormatInternal, typename ... ColorTextures>
class GenericFrameBuffer : public Handle
{
public:

  using DepthStencilTexture = GenericTexture<GL_TEXTURE_2D, Layout, Format, FormatInternal>;

public:

  constexpr static inline U32 BufferAttachmentTypes[] =
  {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
  };

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
    ((glFramebufferTexture2D(GL_FRAMEBUFFER, BufferAttachmentTypes[bufferAttachmentIndex++], GL_TEXTURE_2D, ((typename TypeProxy<ColorTextures>::Ptr)mColorTextures[colorTextureIndex++])->GetID(), 0)), ...);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilTexture->GetID(), 0);
    for (U32 i = 0; i < (sizeof ... (ColorTextures)); ++i)
    {
      bufferAttachments[i] = BufferAttachmentTypes[i];
    }
    glDrawBuffers(1, bufferAttachments);
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