#include <Filesystem/HotLoader.h>

#include <Vertex.h>

#include <Handles/GenericBuffer.h>
#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>
#include <Handles/GenericShader.h>
#include <Handles/GenericTexture.h>

HotLoader::HotLoader(
  GLFWwindow* window,
  World* world,
  std::filesystem::path const& searchPath)
  : mWindow{ window }
  , mWorld{ world }
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
}

void HotLoader::UpdateModules()
{
  mWatchdogModule.Update();

  for (auto const& file : mWatchdogModule.FilesToDelete())
  {
    std::filesystem::path tempFilePath = mTempFolder / "Modules" / file.filename().string();
    if (mWorld->DestroyModule(file.stem().string()))
    {
      std::filesystem::remove(tempFilePath);
    }
  }

  for (auto const& file : mWatchdogModule.FilesToChange())
  {
    std::filesystem::path tempFilePath = mTempFolder / "Modules" / file.filename().string();
    if (mWorld->DestroyModule(file.stem().string()))
    {
      std::filesystem::remove(tempFilePath);
      std::filesystem::copy(file, tempFilePath);
      if (mWorld->CreateModule(tempFilePath))
      {

      }
    }
  }

  for (auto const& file : mWatchdogModule.FilesToCreate())
  {
    std::filesystem::path tempFilePath = mTempFolder / "Modules" / file.filename().string();
    std::filesystem::copy(file, tempFilePath);
    if (mWorld->CreateModule(tempFilePath))
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
    mWorld->UnMountResource<TextureResource>(assetName);
    if (Texture2DU8RGB* texture = mWorld->GetHandleUnsafe<Texture2DU8RGB>(assetName)) texture->SetDirty(true);
    if (Texture2DU8RGBA* texture = mWorld->GetHandleUnsafe<Texture2DU8RGBA>(assetName)) texture->SetDirty(true);
    if (Texture2DR32RGB* texture = mWorld->GetHandleUnsafe<Texture2DR32RGB>(assetName)) texture->SetDirty(true);
    if (Texture2DR32RGBA* texture = mWorld->GetHandleUnsafe<Texture2DR32RGBA>(assetName)) texture->SetDirty(true);
    ReMountTextureHandles();
  }

  for (auto const& file : mWatchdogTexture.FilesToChange())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<TextureResource>(assetName);
    if (Texture2DU8RGB* texture = mWorld->GetHandleUnsafe<Texture2DU8RGB>(assetName)) texture->SetDirty(true);
    if (Texture2DU8RGBA* texture = mWorld->GetHandleUnsafe<Texture2DU8RGBA>(assetName)) texture->SetDirty(true);
    if (Texture2DR32RGB* texture = mWorld->GetHandleUnsafe<Texture2DR32RGB>(assetName)) texture->SetDirty(true);
    if (Texture2DR32RGBA* texture = mWorld->GetHandleUnsafe<Texture2DR32RGBA>(assetName)) texture->SetDirty(true);
    ReMountTextureHandles();
    mWorld->MountResource<TextureResource>(assetName, file);
    ReMountTextureHandles();
  }

  for (auto const& file : mWatchdogTexture.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    mWorld->MountResource<TextureResource>(assetName, file);
    ReMountTextureHandles();
  }
}
void HotLoader::UpdateMesheAssets()
{
  mWatchdogMesh.Update();

  for (auto const& file : mWatchdogMesh.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<MeshResource>(assetName);
    if (VertexBuffer<Vertex>* vertexBuffer = mWorld->GetHandleUnsafe<VertexBuffer<Vertex>>(assetName)) vertexBuffer->SetDirty(true);
    if (ElementBuffer<U32>* elementBuffer = mWorld->GetHandleUnsafe<ElementBuffer<U32>>(assetName)) elementBuffer->SetDirty(true);
    if (DefaultMesh* mesh = mWorld->GetHandleUnsafe<DefaultMesh>(assetName)) mesh->SetDirty(true);
    ReMountMeshHandles();
  }

  for (auto const& file : mWatchdogMesh.FilesToChange())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<MeshResource>(assetName);
    if (VertexBuffer<Vertex>* vertexBuffer = mWorld->GetHandleUnsafe<VertexBuffer<Vertex>>(assetName)) vertexBuffer->SetDirty(true);
    if (ElementBuffer<U32>* elementBuffer = mWorld->GetHandleUnsafe<ElementBuffer<U32>>(assetName)) elementBuffer->SetDirty(true);
    if (DefaultMesh* mesh = mWorld->GetHandleUnsafe<DefaultMesh>(assetName)) mesh->SetDirty(true);
    ReMountMeshHandles();
    mWorld->MountResource<MeshResource>(assetName, file);
    ReMountMeshHandles();
  }

  for (auto const& file : mWatchdogMesh.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    mWorld->MountResource<MeshResource>(assetName, file);
    ReMountMeshHandles();
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
    mWorld->UnMountResource<ShaderResource<EShaderType::Vertex>>(assetName);
    if (VertexShader* vertexShader = mWorld->GetHandleUnsafe<VertexShader>(assetName)) vertexShader->SetDirty(true);
    if (RenderProgram* renderProgram = mWorld->GetHandleUnsafe<RenderProgram>(assetName)) renderProgram->SetDirty(true);
    ReMountRenderProgramHandles();
  }
  for (auto const& file : mWatchdogFragmentShader.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<ShaderResource<EShaderType::Fragment>>(assetName);
    if (FragmentShader* fragmentShader = mWorld->GetHandleUnsafe<FragmentShader>(assetName)) fragmentShader->SetDirty(true);
    if (RenderProgram* renderProgram = mWorld->GetHandleUnsafe<RenderProgram>(assetName)) renderProgram->SetDirty(true);
    ReMountRenderProgramHandles();
  }
  for (auto const& file : mWatchdogComputeShader.FilesToDelete())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<ShaderResource<EShaderType::Compute>>(assetName);
    if (ComputeShader* computeShader = mWorld->GetHandleUnsafe<ComputeShader>(assetName)) computeShader->SetDirty(true);
    if (ComputeProgram* computeProgram = mWorld->GetHandleUnsafe<ComputeProgram>(assetName)) computeProgram->SetDirty(true);
    ReMountComputeProgramHandles();
  }

  for (auto const& file : mWatchdogVertexShader.FilesToChange())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<ShaderResource<EShaderType::Vertex>>(assetName);
    if (VertexShader* vertexShader = mWorld->GetHandleUnsafe<VertexShader>(assetName)) vertexShader->SetDirty(true);
    if (RenderProgram* renderProgram = mWorld->GetHandleUnsafe<RenderProgram>(assetName)) renderProgram->SetDirty(true);
    ReMountRenderProgramHandles();
    mWorld->MountResource<ShaderResource<EShaderType::Vertex>>(assetName, file);
    ReMountRenderProgramHandles();
  }
  for (auto const& file : mWatchdogFragmentShader.FilesToChange())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<ShaderResource<EShaderType::Fragment>>(assetName);
    if (FragmentShader* fragmentShader = mWorld->GetHandleUnsafe<FragmentShader>(assetName)) fragmentShader->SetDirty(true);
    if (RenderProgram* renderProgram = mWorld->GetHandleUnsafe<RenderProgram>(assetName)) renderProgram->SetDirty(true);
    ReMountRenderProgramHandles();
    mWorld->MountResource<ShaderResource<EShaderType::Fragment>>(assetName, file);
    ReMountRenderProgramHandles();
  }
  for (auto const& file : mWatchdogComputeShader.FilesToChange())
  {
    std::string assetName = file.stem().string();
    mWorld->UnMountResource<ShaderResource<EShaderType::Compute>>(assetName);
    if (ComputeShader* computeShader = mWorld->GetHandleUnsafe<ComputeShader>(assetName)) computeShader->SetDirty(true);
    if (ComputeProgram* computeProgram = mWorld->GetHandleUnsafe<ComputeProgram>(assetName)) computeProgram->SetDirty(true);
    ReMountComputeProgramHandles();
    mWorld->MountResource<ShaderResource<EShaderType::Compute>>(assetName, file);
    ReMountComputeProgramHandles();
  }

  for (auto const& file : mWatchdogVertexShader.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    mWorld->MountResource<ShaderResource<EShaderType::Vertex>>(assetName, file);
    ReMountRenderProgramHandles();
  }
  for (auto const& file : mWatchdogFragmentShader.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    mWorld->MountResource<ShaderResource<EShaderType::Fragment>>(assetName, file);
    ReMountRenderProgramHandles();
  }
  for (auto const& file : mWatchdogComputeShader.FilesToCreate())
  {
    std::string assetName = file.stem().string();
    mWorld->MountResource<ShaderResource<EShaderType::Compute>>(assetName, file);
    ReMountComputeProgramHandles();
  }
}

void HotLoader::ReMountRenderProgramHandles()
{
  //// Find dirty render shader names
  //std::set<std::string> dirtyShaderList;
  //mWorld->CollectDirtyHandleNamesByType<VertexShader>(dirtyShaderList);
  //mWorld->CollectDirtyHandleNamesByType<FragmentShader>(dirtyShaderList);
  //// Find non dirty render shader names
  //std::set<std::string> nonDirtyShaderList;
  //mWorld->CollectNonDirtyHandleNamesByType<VertexShader>(nonDirtyShaderList);
  //mWorld->CollectNonDirtyHandleNamesByType<FragmentShader>(nonDirtyShaderList);
  //// Push new render program handle
  //for (auto const& name : nonDirtyShaderList)
  //{
  //  VertexShader* vertexShader = mWorld->GetFirstNonDirtyHandleByName<VertexShader>(name);
  //  FragmentShader* fragmentShader = mWorld->GetFirstNonDirtyHandleByName<FragmentShader>(name);
  //  if (vertexShader && fragmentShader)
  //  {
  //    if (mWorld->MountHandle<RenderProgram>(name)->Link(vertexShader, fragmentShader))
  //    {
  //      std::printf("Exchange VertexShader & FragmentShader for RenderProgram\n");
  //      //mWorld->DestroyHandle<VertexShader>(name);
  //      //mWorld->DestroyHandle<FragmentShader>(name);
  //    }
  //  }
  //}
  //// Erase dirty render shaders
  //for (auto const& name : dirtyShaderList)
  //{
  //  //mWorld->DestroyHandle<VertexShader>(name);
  //  //mWorld->DestroyHandle<FragmentShader>(name);
  //}
  //// Find dirty render program names
  //std::set<std::string> dirtyProgramList;
  //mWorld->CollectDirtyHandleNamesByType<RenderProgram>(dirtyProgramList);
  //// Erase dirty render program handles
  //for (auto const& name : dirtyProgramList)
  //{
  //  //mWorld->DestroyHandle<RenderProgram>(name);
  //}
}
void HotLoader::ReMountComputeProgramHandles()
{
  //// Find dirty compute shader names
  //std::set<std::string> dirtyComputeList;
  //mWorld->CollectDirtyHandleNamesByType<ComputeShader>(dirtyComputeList);
  //// Find non dirty compute shader names
  //std::set<std::string> nonDirtyComputeList;
  //mWorld->CollectNonDirtyHandleNamesByType<ComputeShader>(nonDirtyComputeList);
  //// Push new compute program handle
  //for (auto const& name : nonDirtyComputeList)
  //{
  //  ComputeShader* computeShader = mWorld->GetFirstNonDirtyHandleByName<ComputeShader>(name);
  //  if (computeShader)
  //  {
  //    if (mWorld->MountHandle<ComputeProgram>(name)->Link(computeShader))
  //    {
  //      std::printf("Exchange ComputeShader for ComputeProgram\n");
  //      //mWorld->DestroyHandle<ComputeShader>(name);
  //    }
  //  }
  //}
  //// Erase dirty compute shaders
  //for (auto const& name : dirtyComputeList)
  //{
  //  //mWorld->DestroyHandle<ComputeShader>(name);
  //}
  //// Find dirty compute program names
  //std::set<std::string> dirtyProgramList;
  //mWorld->CollectDirtyHandleNamesByType<ComputeProgram>(dirtyProgramList);
  //// Erase dirty compute program handles
  //for (auto const& name : dirtyProgramList)
  //{
  //  //mWorld->DestroyHandle<ComputeProgram>(name);
  //}
}
void HotLoader::ReMountTextureHandles()
{
  //// Find dirty texture names
  //std::set<std::string> dirtyTexture2DU8RGBList;
  //std::set<std::string> dirtyTexture2DU8RGBAList;
  //std::set<std::string> dirtyTexture2DR32RGBList;
  //std::set<std::string> dirtyTexture2DR32RGBAList;
  //mWorld->CollectDirtyHandleNamesByType<Texture2DU8RGB>(dirtyTexture2DU8RGBList);
  //mWorld->CollectDirtyHandleNamesByType<Texture2DU8RGBA>(dirtyTexture2DU8RGBAList);
  //mWorld->CollectDirtyHandleNamesByType<Texture2DR32RGB>(dirtyTexture2DR32RGBList);
  //mWorld->CollectDirtyHandleNamesByType<Texture2DR32RGBA>(dirtyTexture2DR32RGBAList);
  //// Find non texture names
  //std::set<std::string> nonDirtyTexture2DU8RGBList;
  //std::set<std::string> nonDirtyTexture2DU8RGBAList;
  //std::set<std::string> nonDirtyTexture2DR32RGBList;
  //std::set<std::string> nonDirtyTexture2DR32RGBAList;
  //mWorld->CollectDirtyHandleNamesByType<Texture2DU8RGB>(nonDirtyTexture2DU8RGBList);
  //mWorld->CollectDirtyHandleNamesByType<Texture2DU8RGBA>(nonDirtyTexture2DU8RGBAList);
  //mWorld->CollectDirtyHandleNamesByType<Texture2DR32RGB>(nonDirtyTexture2DR32RGBList);
  //mWorld->CollectDirtyHandleNamesByType<Texture2DR32RGBA>(nonDirtyTexture2DR32RGBAList);
  //// Erase dirty textures
  //for (auto const& name : dirtyTexture2DU8RGBList)
  //{
  //  //mWorld->DestroyHandle<Texture2DU8RGB>(name);
  //}
  //for (auto const& name : dirtyTexture2DU8RGBAList)
  //{
  //  //mWorld->DestroyHandle<Texture2DU8RGBA>(name);
  //}
  //for (auto const& name : dirtyTexture2DR32RGBList)
  //{
  //  //mWorld->DestroyHandle<Texture2DR32RGB>(name);
  //}
  //for (auto const& name : dirtyTexture2DR32RGBAList)
  //{
  //  //mWorld->DestroyHandle<Texture2DR32RGBA>(name);
  //}
}
void HotLoader::ReMountMeshHandles()
{
  CreateMeshHandles();
  MergeMeshHandles();
}

void HotLoader::CreateMeshHandles()
{
  //// Find non dirty buffer names
  //std::set<std::string> nonDirtyBufferList;
  //mWorld->CollectNonDirtyHandleNamesByType<VertexBuffer<Vertex>>(nonDirtyBufferList);
  //mWorld->CollectNonDirtyHandleNamesByType<ElementBuffer<U32>>(nonDirtyBufferList);
  //// Push new mesh handles
  //for (auto const& name : nonDirtyBufferList)
  //{
  //  VertexBuffer<Vertex>* vertexBuffer = mWorld->GetFirstNonDirtyHandleByName<VertexBuffer<Vertex>>(name);
  //  ElementBuffer<U32>* elementBuffer = mWorld->GetFirstNonDirtyHandleByName<ElementBuffer<U32>>(name);
  //  if (vertexBuffer && elementBuffer)
  //  {
  //    // Create new handle and link VBO/EBO to VAO
  //    DefaultMesh* newMesh = mWorld->MountHandle<DefaultMesh>(name);
  //    if (newMesh->Link(vertexBuffer, elementBuffer))
  //    {
  //      newMesh->SetDirty(false);
  //      vertexBuffer->SetDirty(true);
  //      elementBuffer->SetDirty(true);
  //    }
  //  }
  //}
}
void HotLoader::MergeMeshHandles()
{
  //// Find dirty buffer names
  //std::set<std::string> dirtyBufferList;
  //mWorld->CollectNonDirtyHandleNamesByType<VertexBuffer<Vertex>>(dirtyBufferList);
  //mWorld->CollectNonDirtyHandleNamesByType<ElementBuffer<U32>>(dirtyBufferList);
  //// Erase dirty buffers
  //for (auto const& name : dirtyBufferList)
  //{
  //  mWorld->DestroyHandle<VertexBuffer<Vertex>>(name);
  //  mWorld->DestroyHandle<ElementBuffer<U32>>(name);
  //}
  //// Find dirty mesh names
  //std::set<std::string> dirtyMeshList;
  //mWorld->CollectDirtyHandleNamesByType<DefaultMesh>(dirtyMeshList);
  //// Erase dirty mesh handles
  //for (auto const& name : dirtyMeshList)
  //{
  //  // When new mesh is available rewire old references to new one
  //  DefaultMesh* newMesh = mWorld->GetFirstNonDirtyHandleByName<DefaultMesh>(name);
  //  if (newMesh)
  //  {
  //    while (DefaultMesh* oldMesh = mWorld->GetFirstDirtyHandleByName<DefaultMesh>(name))
  //    {
  //      //oldMesh->GetActors()
  //      if (oldMesh->GetReferenceCount() > 0)
  //      {
  //
  //      }
  //    }
  //  }
  //}
}