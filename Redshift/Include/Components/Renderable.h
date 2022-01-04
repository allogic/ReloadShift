#pragma once

#include <Core.h>
#include <Component.h>
#include <World.h>

#include <Handles/GenericMesh.h>

class Renderable : public Component
{
public:

  Renderable(World* world, std::string const& meshName)
    : Component(world)
    , mMeshName{ meshName }
  {
    MountHandles();
  }

public:

  virtual void MountHandles() override
  {
    mMesh = mWorld->GetFirstNonDirtyHandleByName<DefaultMesh>(mMeshName);
    mMesh->IncReferenceCount();
  }
  virtual void UnMountHandles() override
  {
    mMesh->DecReferenceCount();
    mMesh = nullptr;
  }

private:

  std::string mMeshName;
  DefaultMesh* mMesh = nullptr;
};