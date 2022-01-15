#include <Renderer.h>

#include <Globals/World.h>

Renderer::Renderer(World& world, std::string const& name)
  : mWorld{ world }
  , mName{ name }
{

}