#include <Resource.h>

#include <Globals/World.h>

Resource::Resource(
  std::string const& name,
  std::filesystem::path const& filePath)
  : mWorld{ World::Instance() }
  , mName{ name }
  , mFilePath{ filePath }
{

}