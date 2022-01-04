#include <World.h>
#include <Module.h>

World::World()
  : mGladContext{ gladGetGLContext() }
  , mImGuiContext{ ImGui::GetCurrentContext() }
{

}

bool World::CreateModule(std::filesystem::path const& filePath)
{
  auto const moduleIt = mModules.find(filePath.stem().string());
  if (moduleIt == mModules.end())
  {
    HINSTANCE libraryInstance = LoadLibraryA(filePath.string().c_str());
    if (libraryInstance)
    {
      Module::CreateProc createProc = (Module::CreateProc)GetProcAddress(libraryInstance, "CreateModule");
      Module::DestroyProc destroyProc = (Module::DestroyProc)GetProcAddress(libraryInstance, "DestroyModule");
      if (createProc && destroyProc)
      {
        Module* moduleInstance = createProc(this);
        if (moduleInstance)
        {
          auto const& [emplaceIt, inserted] = mModules.emplace(filePath.stem().string(), new ModuleProxy{ libraryInstance, moduleInstance, createProc, destroyProc });
          return inserted;
        }
      }
    }
  }
  return false;
}
bool World::DestroyModule(std::string const& moduleName)
{
  auto const moduleIt = mModules.find(moduleName);
  if (moduleIt != mModules.end())
  {
    ModuleProxy* proxy = moduleIt->second;
    proxy->GetDestroyProx()(proxy->GetModInstance());
    FreeLibrary(proxy->GetWinInstance());
    delete proxy;
    mModules.erase(moduleIt);
    return true;
  }
  return false;
}