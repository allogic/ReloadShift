#include <Filesystem/HotLoader.h>

#include <Vertex.h>

#include <Handles/GenericBuffer.h>
#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>
#include <Handles/GenericShader.h>
#include <Handles/GenericTexture.h>

HotLoader::HotLoader(
  GLFWwindow* window,
  std::filesystem::path const& searchPath)
  : mWindow{ window }
  , mWorld{ World::Instance() }
  , mTempFolder{ std::filesystem::temp_directory_path() / "Redshift" }
  , mWatchdogModule{ searchPath / "Modules", ".dll" }
  , mWatchdogTexture{ searchPath / "Textures", ".png" }
  , mWatchdogMesh{ searchPath / "Meshes", ".fbx" }
  , mWatchdogVertexShader{ searchPath / "Shaders", ".vert" }
  , mWatchdogFragmentShader{ searchPath / "Shaders", ".frag" }
  , mWatchdogComputeShader{ searchPath / "Shaders", ".comp" }
{
  std::filesystem::create_directory(searchPath / "Modules");
  std::filesystem::create_directory(searchPath / "Textures");
  std::filesystem::create_directory(searchPath / "Meshes");
  std::filesystem::create_directory(searchPath / "Shaders");

  std::filesystem::create_directory(mTempFolder / "Modules");

  for (auto const& file : std::filesystem::directory_iterator(mTempFolder / "Modules"))
  {
    if (!file.is_directory())
    {
      std::filesystem::remove(file);
    }
  }
}

void HotLoader::Update()
{
  UpdateModules();

  UpdateTexturesAssets();
  UpdateMesheAssets();
  UpdateShadersAssets();

  ProduceRenderProgramHandles();
  ProduceComputeProgramHandles();
  ProduceTextureHandles();
  ProduceMeshHandles();
}

void HotLoader::UpdateModules()
{
  mWatchdogModule.Update();

  for (auto const& file : mWatchdogModule.FilesToDelete())
  {
    std::filesystem::path tempFilePath = mTempFolder / "Modules" / file.filename().string();
    if (World::DestroyModule(mWorld, file.stem().string()))
    {
      std::filesystem::remove(tempFilePath);
    }
  }

  for (auto const& file : mWatchdogModule.FilesToChange())
  {
    std::filesystem::path tempFilePath = mTempFolder / "Modules" / file.filename().string();
    if (World::DestroyModule(mWorld, file.stem().string()))
    {
      std::filesystem::remove(tempFilePath);
      std::filesystem::copy(file, tempFilePath);
      if (World::CreateModule(mWorld, tempFilePath))
      {

      }
    }
  }

  for (auto const& file : mWatchdogModule.FilesToCreate())
  {
    std::filesystem::path tempFilePath = mTempFolder / "Modules" / file.filename().string();
    std::filesystem::copy(file, tempFilePath);
    if (World::CreateModule(mWorld, tempFilePath))
    {

    }
  }
}

void HotLoader::UpdateTexturesAssets()
{
  mWatchdogTexture.Update();

  for (auto const& file : mWatchdogTexture.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<TextureResource>(mWorld, assetName);
    World::TrySetDirty<Texture2DU8RGB>(mWorld, assetName);
    World::TrySetDirty<Texture2DU8RGBA>(mWorld, assetName);
    World::TrySetDirty<Texture2DR32RGB>(mWorld, assetName);
    World::TrySetDirty<Texture2DR32RGBA>(mWorld, assetName);
  }

  for (auto const& file : mWatchdogTexture.FilesToChange())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<TextureResource>(mWorld, assetName);
    World::TrySetDirty<Texture2DU8RGB>(mWorld, assetName);
    World::TrySetDirty<Texture2DU8RGBA>(mWorld, assetName);
    World::TrySetDirty<Texture2DR32RGB>(mWorld, assetName);
    World::TrySetDirty<Texture2DR32RGBA>(mWorld, assetName);
    World::CreateResource<TextureResource>(mWorld, assetName, file);
  }

  for (auto const& file : mWatchdogTexture.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    World::CreateResource<TextureResource>(mWorld, assetName, file);
  }
}
void HotLoader::UpdateMesheAssets()
{
  mWatchdogMesh.Update();

  for (auto const& file : mWatchdogMesh.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<MeshResource>(mWorld, assetName);
    World::TrySetDirty<VertexBuffer<Vertex>>(mWorld, assetName);
    World::TrySetDirty<ElementBuffer<U32>>(mWorld, assetName);
    World::TrySetDirty<DefaultMesh>(mWorld, assetName);
    ProduceMeshHandles();
  }

  for (auto const& file : mWatchdogMesh.FilesToChange())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<MeshResource>(mWorld, assetName);
    World::TrySetDirty<VertexBuffer<Vertex>>(mWorld, assetName);
    World::TrySetDirty<ElementBuffer<U32>>(mWorld, assetName);
    World::TrySetDirty<DefaultMesh>(mWorld, assetName);
    ProduceMeshHandles();
    World::CreateResource<MeshResource>(mWorld, assetName, file);
    ProduceMeshHandles();
  }

  for (auto const& file : mWatchdogMesh.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    World::CreateResource<MeshResource>(mWorld, assetName, file);
    ProduceMeshHandles();
  }
}
void HotLoader::UpdateShadersAssets()
{
  mWatchdogVertexShader.Update();
  mWatchdogFragmentShader.Update();
  mWatchdogComputeShader.Update();

  for (auto const& file : mWatchdogVertexShader.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<ShaderResource<EShaderType::Vertex>>(mWorld, assetName);
    World::TrySetDirty<VertexShader>(mWorld, assetName);
    World::TrySetDirty<RenderProgram>(mWorld, assetName);
    ProduceRenderProgramHandles();
  }
  for (auto const& file : mWatchdogFragmentShader.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<ShaderResource<EShaderType::Fragment>>(mWorld, assetName);
    World::TrySetDirty<FragmentShader>(mWorld, assetName);
    World::TrySetDirty<RenderProgram>(mWorld, assetName);
    ProduceRenderProgramHandles();
  }
  for (auto const& file : mWatchdogComputeShader.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<ShaderResource<EShaderType::Compute>>(mWorld, assetName);
    World::TrySetDirty<ComputeShader>(mWorld, assetName);
    World::TrySetDirty<ComputeProgram>(mWorld, assetName);
    ProduceComputeProgramHandles();
  }

  for (auto const& file : mWatchdogVertexShader.FilesToChange())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<ShaderResource<EShaderType::Vertex>>(mWorld, assetName);
    World::TrySetDirty<VertexShader>(mWorld, assetName);
    World::TrySetDirty<RenderProgram>(mWorld, assetName);
    ProduceRenderProgramHandles();
    World::CreateResource<ShaderResource<EShaderType::Vertex>>(mWorld, assetName, file);
    ProduceRenderProgramHandles();
  }
  for (auto const& file : mWatchdogFragmentShader.FilesToChange())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<ShaderResource<EShaderType::Fragment>>(mWorld, assetName);
    World::TrySetDirty<FragmentShader>(mWorld, assetName);
    World::TrySetDirty<RenderProgram>(mWorld, assetName);
    ProduceRenderProgramHandles();
    World::CreateResource<ShaderResource<EShaderType::Fragment>>(mWorld, assetName, file);
    ProduceRenderProgramHandles();
  }
  for (auto const& file : mWatchdogComputeShader.FilesToChange())
  {
    std::string assetName = file.stem().string();
    World::DestroyResource<ShaderResource<EShaderType::Compute>>(mWorld, assetName);
    World::TrySetDirty<ComputeShader>(mWorld, assetName);
    World::TrySetDirty<ComputeProgram>(mWorld, assetName);
    ProduceComputeProgramHandles();
    World::CreateResource<ShaderResource<EShaderType::Compute>>(mWorld, assetName, file);
    ProduceComputeProgramHandles();
  }

  for (auto const& file : mWatchdogVertexShader.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    World::CreateResource<ShaderResource<EShaderType::Vertex>>(mWorld, assetName, file);
    ProduceRenderProgramHandles();
  }
  for (auto const& file : mWatchdogFragmentShader.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    World::CreateResource<ShaderResource<EShaderType::Fragment>>(mWorld, assetName, file);
    ProduceRenderProgramHandles();
  }
  for (auto const& file : mWatchdogComputeShader.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    World::CreateResource<ShaderResource<EShaderType::Compute>>(mWorld, assetName, file);
    ProduceComputeProgramHandles();
  }
}

void HotLoader::ProduceRenderProgramHandles()
{
  // Find non dirty render shader names
  std::set<std::string> nonDirtyShaderList;
  World::ConsumeNonDirtyHandleNames<VertexShader>(mWorld, nonDirtyShaderList);
  World::ConsumeNonDirtyHandleNames<FragmentShader>(mWorld, nonDirtyShaderList);
  // Push new render program handles
  for (auto const& name : nonDirtyShaderList)
  {
    HotRef<VertexShader>& vertexShaderRef = World::GetHandle<VertexShader>(mWorld, name);
    HotRef<FragmentShader>& fragmentShaderRef = World::GetHandle<FragmentShader>(mWorld, name);
    if (vertexShaderRef.Get() && fragmentShaderRef.Get())
    {
      HotRef<RenderProgram>& programRef = World::GetHandle<RenderProgram>(mWorld, name);
      if (programRef.Get())
      {
        // Compare old values and decide if it has been changed at all
      }
      else
      {
        RenderProgram* programHandle = new RenderProgram{ name };
        if (programHandle->Link(vertexShaderRef.Get(), fragmentShaderRef.Get()))
        {
          programHandle->AddReference(this);
          programRef.Set(programHandle);
        }
        else
        {
          delete programHandle;
        }
      }
    }
  }
}
void HotLoader::ProduceComputeProgramHandles()
{
  //// Find dirty compute shader names
  //std::set<std::string> dirtyComputeList;
  //mWorld.CollectDirtyHandleNamesByType<ComputeShader>(dirtyComputeList);
  //// Find non dirty compute shader names
  //std::set<std::string> nonDirtyComputeList;
  //mWorld.CollectNonDirtyHandleNamesByType<ComputeShader>(nonDirtyComputeList);
  //// Push new compute program handle
  //for (auto const& name : nonDirtyComputeList)
  //{
  //  ComputeShader* computeShader = mWorld.GetFirstNonDirtyHandleByName<ComputeShader>(name);
  //  if (computeShader)
  //  {
  //    if (mWorld.MountHandle<ComputeProgram>(name)->Link(computeShader))
  //    {
  //      std::printf("Exchange ComputeShader for ComputeProgram\n");
  //      //mWorld.DestroyHandle<ComputeShader>(name);
  //    }
  //  }
  //}
  //// Erase dirty compute shaders
  //for (auto const& name : dirtyComputeList)
  //{
  //  //mWorld.DestroyHandle<ComputeShader>(name);
  //}
  //// Find dirty compute program names
  //std::set<std::string> dirtyProgramList;
  //mWorld.CollectDirtyHandleNamesByType<ComputeProgram>(dirtyProgramList);
  //// Erase dirty compute program handles
  //for (auto const& name : dirtyProgramList)
  //{
  //  //mWorld.DestroyHandle<ComputeProgram>(name);
  //}
}
void HotLoader::ProduceTextureHandles()
{
  //// Find dirty texture names
  //std::set<std::string> dirtyTexture2DU8RGBList;
  //std::set<std::string> dirtyTexture2DU8RGBAList;
  //std::set<std::string> dirtyTexture2DR32RGBList;
  //std::set<std::string> dirtyTexture2DR32RGBAList;
  //mWorld.CollectDirtyHandleNamesByType<Texture2DU8RGB>(dirtyTexture2DU8RGBList);
  //mWorld.CollectDirtyHandleNamesByType<Texture2DU8RGBA>(dirtyTexture2DU8RGBAList);
  //mWorld.CollectDirtyHandleNamesByType<Texture2DR32RGB>(dirtyTexture2DR32RGBList);
  //mWorld.CollectDirtyHandleNamesByType<Texture2DR32RGBA>(dirtyTexture2DR32RGBAList);
  //// Find non texture names
  //std::set<std::string> nonDirtyTexture2DU8RGBList;
  //std::set<std::string> nonDirtyTexture2DU8RGBAList;
  //std::set<std::string> nonDirtyTexture2DR32RGBList;
  //std::set<std::string> nonDirtyTexture2DR32RGBAList;
  //mWorld.CollectDirtyHandleNamesByType<Texture2DU8RGB>(nonDirtyTexture2DU8RGBList);
  //mWorld.CollectDirtyHandleNamesByType<Texture2DU8RGBA>(nonDirtyTexture2DU8RGBAList);
  //mWorld.CollectDirtyHandleNamesByType<Texture2DR32RGB>(nonDirtyTexture2DR32RGBList);
  //mWorld.CollectDirtyHandleNamesByType<Texture2DR32RGBA>(nonDirtyTexture2DR32RGBAList);
  //// Erase dirty textures
  //for (auto const& name : dirtyTexture2DU8RGBList)
  //{
  //  //mWorld.DestroyHandle<Texture2DU8RGB>(name);
  //}
  //for (auto const& name : dirtyTexture2DU8RGBAList)
  //{
  //  //mWorld.DestroyHandle<Texture2DU8RGBA>(name);
  //}
  //for (auto const& name : dirtyTexture2DR32RGBList)
  //{
  //  //mWorld.DestroyHandle<Texture2DR32RGB>(name);
  //}
  //for (auto const& name : dirtyTexture2DR32RGBAList)
  //{
  //  //mWorld.DestroyHandle<Texture2DR32RGBA>(name);
  //}
}
void HotLoader::ProduceMeshHandles()
{
  // Find non dirty buffer names
  std::set<std::string> nonDirtyBufferList;
  World::ConsumeNonDirtyHandleNames<VertexBuffer<Vertex>>(mWorld, nonDirtyBufferList);
  World::ConsumeNonDirtyHandleNames<ElementBuffer<U32>>(mWorld, nonDirtyBufferList);
  // Push new mesh handles
  for (auto const& name : nonDirtyBufferList)
  {
    HotRef<VertexBuffer<Vertex>>& vertexBufferRef = World::GetHandle<VertexBuffer<Vertex>>(mWorld, name);
    HotRef<ElementBuffer<U32>>& elementBufferRef = World::GetHandle<ElementBuffer<U32>>(mWorld, name);
    if (vertexBufferRef.Get() && elementBufferRef.Get())
    {
      HotRef<DefaultMesh>& meshRef = World::GetHandle<DefaultMesh>(mWorld, name);
      if (meshRef.Get())
      {
        // Compare old values and decide if it has been changed at all
      }
      else
      {
        DefaultMesh* meshHandle = new DefaultMesh{ name };
        if (meshHandle->Link(vertexBufferRef.Get(), elementBufferRef.Get()))
        {
          meshHandle->AddReference(this);
          meshRef.Set(meshHandle);
        }
        else
        {
          delete meshHandle;
        }
      }
    }
  }
}