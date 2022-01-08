#pragma once

#include <Core.h>
#include <Component.h>
#include <World.h>

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
    MountHandles();
  }

public:

  virtual void MountHandles() override
  {
    mMesh = mWorld->GetFirstNonDirtyHandleByName<DefaultMesh>(mMeshName);
    mProgram = mWorld->GetFirstNonDirtyHandleByName<RenderProgram>(mProgramName);

    if (mMesh) mMesh->IncReferenceCount();
    if (mProgram) mProgram->IncReferenceCount();
  }
  virtual void UnMountHandles() override
  {
    if (mMesh) mMesh->DecReferenceCount(); mMesh = nullptr;
    if (mProgram) mProgram->DecReferenceCount(); mProgram = nullptr;
  }

public:

  void SubmitDrawCall(Transform* transform, std::queue<RenderTask>& renderQueue)
  {
    if (transform && mMesh && mProgram)
    {
      renderQueue.emplace(RenderTask{ transform, mMesh, mProgram });
    }
  }

private:

  std::string mMeshName;
  std::string mProgramName;

  DefaultMesh* mMesh = nullptr;
  RenderProgram* mProgram = nullptr;
};