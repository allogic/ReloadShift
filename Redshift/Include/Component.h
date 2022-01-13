#pragma once

#include <Core.h>

class World;

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