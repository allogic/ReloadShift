#pragma once

#include <Core.h>
#include <Component.h>

class SphereCollider : public Component
{
public:

  SphereCollider(World& world)
    : Component(world)
  {

  }

private:

  btSphereShape mShape = btSphereShape{ 1.0f };
};