#pragma once

#include <Core.h>

#include <Globals/World.h>

class Component
{
public:

  Component(World& world)
    : mWorld{ world }
  {

  }
  virtual ~Component() = default;

protected:

  World& mWorld;
};