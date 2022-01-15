#include <Globals/World.h>

#include <Module.h>


World::World()
  : mGlfwContext{ glfwGetCurrentContext() }
  , mGladContext{ gladGetGLContext() }
  , mImGuiContext{ ImGui::GetCurrentContext() }
{
  // Setup gravity
  mDynamicsWorld.setGravity(btVector3(0, -5, 0));
}

bool World::CreateModule(World& world, std::filesystem::path const& filePath)
{
  auto const moduleIt = world.mModules.find(filePath.stem().string());
  if (moduleIt == world.mModules.end())
  {
    HINSTANCE libraryInstance = LoadLibraryA(filePath.string().c_str());
    if (libraryInstance)
    {
      Module::CreateProc createProc = (Module::CreateProc)GetProcAddress(libraryInstance, "CreateModule");
      Module::DestroyProc destroyProc = (Module::DestroyProc)GetProcAddress(libraryInstance, "DestroyModule");
      if (createProc && destroyProc)
      {
        Module* moduleInstance = createProc(world);
        if (moduleInstance)
        {
          auto const& [emplaceIt, inserted] = world.mModules.emplace(filePath.stem().string(), new ModuleProxy{ libraryInstance, moduleInstance, createProc, destroyProc });
          return inserted;
        }
      }
    }
  }
  return false;
}
bool World::DestroyModule(World& world, std::string const& moduleName)
{
  auto const moduleIt = world.mModules.find(moduleName);
  if (moduleIt != world.mModules.end())
  {
    ModuleProxy* proxy = moduleIt->second;
    proxy->GetDestroyProx()(proxy->GetModInstance());
    FreeLibrary(proxy->GetWinInstance());
    delete proxy;
    world.mModules.erase(moduleIt);
    return true;
  }
  return false;
}