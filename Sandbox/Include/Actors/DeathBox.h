#pragma once

#include <Redshift.h>

#include <Components/Renderable.h>
#include <Components/Rigidbody.h>

class DeathBox : public Actor
{
public:

  DeathBox(
    World& world,
    ActorProxy* proxy,
    std::string const& name)
    : Actor(world, proxy, name)
    , mTransform{ World::AttachComponent<Transform>(mWorld, this, R32V3{ 0.0f }, R32V3{ 0.0f }, R32V3{ 0.008f }) }
    , mRenderable{ World::AttachComponent<Renderable>(mWorld, this, "Cube", "Lit") }
    , mRigidbody{ World::AttachComponent<Rigidbody>(mWorld, this, 0.0f) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline Rigidbody* GetRigidbody() const { return mRigidbody; }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  Rigidbody* mRigidbody;
};