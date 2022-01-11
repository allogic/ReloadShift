#include <Renderer.h>

#include <Globals/World.h>

Renderer::Renderer(std::string const& name)
  : mWorld{ World::Instance() }
  , mName{ name }
{

}