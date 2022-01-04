#pragma once

#include <Core.h>
#include <Component.h>

class Camera : public Component
{
public:

  Camera(
    World* world,
    R32 FOV,
    R32 near,
    R32 far)
    : Component(world)
    , mFOV{ FOV }
    , mNear{ near }
    , mFar{ far }
  {

  }

private:

  R32 mFOV;
  R32 mNear;
  R32 mFar;
};