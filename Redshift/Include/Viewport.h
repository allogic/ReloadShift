#pragma once

#include <Core.h>

#include <Handles/GenericTexture.h>

template<U32 Layout, U32 Format, U32 FormatInternal>
class Viewport
{
public:

  Viewport(
    std::string const& name,
    GenericTexture<GL_TEXTURE_2D, Layout, Format, FormatInternal>* texture)
    : mName{ name }
    , mTexture{ texture }
  {

  }

public:

  void Draw()
  {
    static bool opened = true;
    ImGui::Begin("Roughness", &opened);
    void* textureID = mTexture ? (void*)(U64)mTexture->GetID() : nullptr;
    ImGui::Image(textureID, ImGui::GetWindowSize());
    ImGui::End();
  }

private:

  std::string mName;
  GenericTexture<GL_TEXTURE_2D, Layout, Format, FormatInternal>* mTexture;
};