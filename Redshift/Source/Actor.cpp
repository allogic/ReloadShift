#include <Actor.h>

#include <Globals/World.h>

Actor::Actor(
  World& world,
  ActorProxy* proxy,
  std::string const& name)
  : mWorld{ world }
  , mProxy{ proxy }
  , mName{ name }
{

}