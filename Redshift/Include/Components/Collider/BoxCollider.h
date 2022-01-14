#pragma once

#include <Core.h>
#include <Component.h>

class BoxCollider : public Component
{
public:

  BoxCollider(World& world)
    : Component(world)
  {

  }

private:

  btBoxShape mShape = btBoxShape{ btVector3{ 1.0f, 1.0f, 1.0f } };
};