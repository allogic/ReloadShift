#pragma once

#include <Core.h>
#include <LightComponent.h>

class SpotLight : public LightComponent
{
public:

  SpotLight(
    World& world,
    R32 intensity,
    R32V3 const& color,
    bool enabled,
    R32 cutOff)
    : LightComponent(world, intensity, color, enabled)
    , mCutOff{ cutOff }
  {

  }

public:

  inline R32 GetCutOff() const { return mCutOff; }

private:

  R32 mCutOff;
};