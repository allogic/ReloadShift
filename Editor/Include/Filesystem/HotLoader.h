#pragma once

#include <Core.h>
#include <Module.h>

#include <Resources/MeshResource.h>
#include <Resources/ShaderResource.h>
#include <Resources/TextureResource.h>

#include <Filesystem/Watchdog.h>

#include <World.h>

class HotLoader
{
public:

  HotLoader(
    GLFWwindow* window,
    World* world,
    std::filesystem::path const& searchPath);

public:

  void Update();

private:

  ////////////////////////////////////////////////////////
  // Streaming binary
  ////////////////////////////////////////////////////////

  void UpdateModules();

private:

  ////////////////////////////////////////////////////////
  // Streaming resources
  ////////////////////////////////////////////////////////

  void UpdateTexturesAssets();
  void UpdateMesheAssets();
  void UpdateShadersAssets();

private:

  ////////////////////////////////////////////////////////
  // Handle specific
  ////////////////////////////////////////////////////////

  void ReMountRenderProgramHandles();
  void ReMountComputeProgramHandles();
  void ReMountTextureHandles();
  void ReMountMeshHandles();

private:

  ////////////////////////////////////////////////////////
  // Hotswapping
  ////////////////////////////////////////////////////////

  void HotswapShaders();
  void HotswapPrograms();

private:

  GLFWwindow* mWindow;
  World* mWorld;
  std::filesystem::path mTempFolder;

  Watchdog mWatchdogModule;
  Watchdog mWatchdogTexture;
  Watchdog mWatchdogMesh;
  Watchdog mWatchdogVertexShader;
  Watchdog mWatchdogFragmentShader;
  Watchdog mWatchdogComputeShader;
};