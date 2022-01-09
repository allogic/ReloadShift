#pragma once

#include <Core.h>

#include <Handles/GenericFrameBuffer.h>
#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>

class World;

class Renderer
{
public:

  Renderer(World* world)
    : mWorld{ world }
  {

  }
  virtual ~Renderer() = default;

public:

  virtual void Render() = 0;

protected:

  World* mWorld;
};