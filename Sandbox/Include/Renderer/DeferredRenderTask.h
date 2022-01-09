#pragma once

#include <Core.h>

#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>

class Transform;

class DeferredRenderTask
{
public:

  DeferredRenderTask(
    Transform* transform,
    DefaultMesh* mesh,
    RenderProgram* program)
    : mTransform{ transform }
    , mMesh{ mesh }
    , mProgram{ program }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline DefaultMesh* GetMesh() const { return mMesh; }
  inline RenderProgram* GetProgram() const { return mProgram; }

public:

  bool operator < (DeferredRenderTask const& other) const { return mProgram < other.mProgram; }

private:

  Transform* mTransform;
  DefaultMesh* mMesh;
  RenderProgram* mProgram;
};