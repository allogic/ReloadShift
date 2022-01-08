#include <Resources/TextureResource.h>
#include <World.h>
#include <Handles/GenericTexture.h>

TextureResource::TextureResource(
  World* world,
  std::string const& name,
  std::filesystem::path const& filePath)
  : Resource(world, name, filePath)
{

}

bool TextureResource::LoadFile()
{
  if (!mBytes)
  {
    I32 width = 0;
    I32 height = 0;
    I32 channels = 0;
    U8* bytes = stbi_load(mFilePath.string().c_str(), &width, &height, &channels, STBI_default);
    if (bytes)
    {
      mBytes = new U8[width * height];
      memcpy(mBytes, bytes, width * height);
      mBytesSize = width * height;
      stbi_image_free(bytes);
      if (width != mWidth) mWidth = width;
      if (height != mHeight) mHeight = height;
      if (channels != mChannels) mChannels = channels;
    }
    return mBytes;
  }
  return false;
}
bool TextureResource::ProduceHandles()
{
  switch (mChannels)
  {
    case 3:
    {
      //mWorld->MarkHandlesAsDirtyByName<Texture2DR32RGB>(GetName());
      //mWorld->MountHandle<Texture2DR32RGB>(GetName(), mWidth, mHeight, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
      //mDirty = false;
      //return true;
    }
    case 4:
    {
      //mWorld->MarkHandlesAsDirtyByName<Texture2DR32RGBA>(GetName());
      //mWorld->MountHandle<Texture2DR32RGBA>(GetName(), mWidth, mHeight, ETextureWrap::ClampToEdge, ETextureFilter::Nearest);
      //mDirty = false;
      //return true;
    }
  }
  return false;
}