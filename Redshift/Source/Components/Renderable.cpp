#include <Components/Renderable.h>
#include <Components/Transform.h>

#include <Globals/World.h>

Renderable::Renderable(
  World& world,
  std::string const& meshName,
  std::string const& programName,
  std::string const& textureAlbedoName,
  std::string const& textureNormalName,
  std::string const& textureSpecularName,
  std::string const& textureMetallicName,
  std::string const& textureRoughnessName,
  std::string const& textureAmbientOcclusionName)
  : Component(world)
  , mMesh{ World::GetHandle<DefaultMesh>(mWorld, meshName) }
  , mProgram{ World::GetHandle<RenderProgram>(mWorld, programName) }
  , mTextureAlbedo{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureAlbedoName) }
  , mTextureNormal{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureNormalName) }
  , mTextureSpecular{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureSpecularName) }
  , mTextureMetallic{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureMetallicName) }
  , mTextureRoughness{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureRoughnessName) }
  , mTextureAmbientOcclusion{ World::GetHandle<Texture2DR32RGBA>(mWorld, textureAmbientOcclusionName) }
{

}

void Renderable::SubmitRenderTask(Transform* transform, std::queue<DeferredRenderTask>& renderQueue)
{
  // Minimum requirements are mesh and program
  if (mMesh.Get() && mProgram.Get())
  {
    renderQueue.emplace(DeferredRenderTask
      {
        transform,
        mMesh.Get(),
        mProgram.Get(),
        mTextureAlbedo.Get(),
        mTextureNormal.Get(),
        mTextureSpecular.Get(),
        mTextureMetallic.Get(),
        mTextureRoughness.Get(),
        mTextureAmbientOcclusion.Get(),
      });
  }
}