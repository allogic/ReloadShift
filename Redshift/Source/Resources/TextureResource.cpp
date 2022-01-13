#include <Resources/TextureResource.h>

#include <Globals/World.h>

#include <Handles/GenericTexture.h>

TextureResource::TextureResource(
  std::string const& name,
  std::filesystem::path const& filePath)
  : Resource(name, filePath)
{

}

bool TextureResource::LoadFile()
{
  I32 width = 0;
  I32 height = 0;
  I32 channels = 0;
  mIntRepresentation = stbi_load(mFilePath.string().c_str(), &width, &height, &channels, STBI_default);
  if (mIntRepresentation)
  {
    mFileSize = width * height * channels;
    mRealRepresentation = new R32[mFileSize];
    for (U32 i = 0; i < mFileSize; ++i)
    {
      mRealRepresentation[i] = (R32)mIntRepresentation[i] / 255.0f;
    }
    if (width != mWidth) mWidth = width;
    if (height != mHeight) mHeight = height;
    if (channels != mChannels) mChannels = channels;
    return true;
  }
  return false;
}
void TextureResource::LinkHandle()
{
  switch (mChannels)
  {
    case 3:
    {
      HotRef<Texture2DR32RGB>& hotRef = World::GetHandle<Texture2DR32RGB>(mWorld, GetName());
      if (hotRef.Get())
      {
        // Compare old values and decide if it has been changed at all
      }
      else
      {
        // Create new texture handle
        Texture2DR32RGB* texture = new Texture2DR32RGB{ GetName(), mWidth, mHeight, ETextureWrap::ClampToEdge, ETextureFilter::Nearest, mRealRepresentation };
        texture->AddReference(this);
        hotRef.Set(texture);
      }
      return;
    }
    case 4:
    {
      HotRef<Texture2DR32RGBA>& hotRef = World::GetHandle<Texture2DR32RGBA>(mWorld, GetName());
      if (hotRef.Get())
      {
        // Compare old values and decide if it has been changed at all
      }
      else
      {
        // Create new texture handle
        Texture2DR32RGBA* texture = new Texture2DR32RGBA{ GetName(), mWidth, mHeight, ETextureWrap::ClampToEdge, ETextureFilter::Nearest, mRealRepresentation };
        texture->AddReference(this);
        hotRef.Set(texture);
      }
      return;
    }
  }
}
void TextureResource::Cleanup()
{
  stbi_image_free(mIntRepresentation); mIntRepresentation = nullptr;
  delete mRealRepresentation; mRealRepresentation = nullptr;  
  mDirty = false;
}