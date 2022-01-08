#pragma once

#include <Core.h>
#include <Component.h>
#include <World.h>
#include <HotRef.h>

#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>

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

  void SubmitRenderTask(Transform* transform, std::queue<RenderTask>& renderQueue)
  {
    if (mMesh.Get() && mProgram.Get())
    {
      renderQueue.emplace(RenderTask{ transform, mMesh.Get(), mProgram.Get() });
    }
  }

private:

  std::string mMeshName;
  std::string mProgramName;

  HotRef<DefaultMesh> const& mMesh = mWorld->LinkHandle<DefaultMesh>(mMeshName);
  HotRef<RenderProgram> const& mProgram = mWorld->LinkHandle<RenderProgram>(mProgramName);
};