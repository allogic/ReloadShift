#pragma once

#include <Core.h>
#include <RenderTask.h>

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

  virtual void PreRender() {};
  virtual void Render() = 0;
  virtual void PostRender() {};

protected:

  World* mWorld;

  std::queue<RenderTask> mRenderQueue = {};
  std::map<RenderProgram*, std::multiset<RenderTask>> mDrawCalls = {};
};