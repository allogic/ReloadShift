#pragma once

#include <Core.h>
#include <Component.h>

class Transform : public Component
{
public:

  Transform(
    World* world,
    R32V3 position,
    R32V3 rotation,
    R32V3 scale)
    : Component(world)
    , mPosition{ position }
    , mRotation{ rotation }
    , mScale{ scale }
  {

  }

private:

  R32V3 mPosition;
  R32V3 mRotation;
  R32V3 mScale;
};