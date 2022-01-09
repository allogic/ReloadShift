#pragma once

#include <Core.h>
#include <Component.h>
#include <World.h>
#include <HotRef.h>

#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>

#include <Renderer/DeferredRenderTask.h>

class Transform;

class Renderable : public Component
{
public:

  Renderable(
    World* world,
    std::string const& meshName,
    std::string const& programName)
    : Component(world)
    , mMeshName{ meshName }
    , mProgramName{ programName }
  {

  }

public:

  void SubmitRenderTask(Transform* transform, std::queue<DeferredRenderTask>& renderQueue)
  {
    if (mMesh.Get() && mProgram.Get())
    {
      renderQueue.emplace(DeferredRenderTask{ transform, mMesh.Get(), mProgram.Get() });
    }
  }

private:

  std::string mMeshName;
  std::string mProgramName;

  HotRef<DefaultMesh> const& mMesh = mWorld->GetHandle<DefaultMesh>(mMeshName);
  HotRef<RenderProgram> const& mProgram = mWorld->GetHandle<RenderProgram>(mProgramName);
};