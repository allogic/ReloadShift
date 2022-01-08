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

public:

  inline R32 GetFOV() const { return mFOV; }
  inline R32 GetNear() const { return mNear; }
  inline R32 GetFar() const { return mFar; }

private:

  R32 mFOV;
  R32 mNear;
  R32 mFar;
};