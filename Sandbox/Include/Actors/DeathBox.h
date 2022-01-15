#pragma once

#include <Redshift.h>

#include <Components/Transform.h>
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
    , mTransform{ World::AttachComponent<Transform>(mWorld, this, R32V3{ 0.0f, 0.0f, 0.0f }, R32V3{ 0.0f }, R32V3{ 5.0f, 1.0f, 5.0f }) }
    , mRenderable{ World::AttachComponent<Renderable>(mWorld, this, "Cube", "Lit") }
    , mCollider{ World::AttachComponent<BoxCollider>(mWorld, this, mTransform) }
    , mRigidbody{ World::AttachComponent<Rigidbody>(mWorld, this, 0.0f, mTransform, mCollider) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline BoxCollider* GetCollider() const { return mCollider; }
  inline Rigidbody* GetRigidbody() const { return mRigidbody; }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  BoxCollider* mCollider;
  Rigidbody* mRigidbody;
};