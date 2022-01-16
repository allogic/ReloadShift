#pragma once

#include <Core.h>
#include <LightComponent.h>

class DirectionLight : public LightComponent
{
public:

  DirectionLight(
    World& world,
    R32 intensity,
    R32V3 const& color,
    bool enabled)
    : LightComponent(world, intensity, color, enabled)
  {

  }
};