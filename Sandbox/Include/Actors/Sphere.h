#pragma once

#include <Redshift.h>

#include <Components/Transform.h>
#include <Components/Renderable.h>
#include <Components/Rigidbody.h>
#include <Components/Collider/SphereCollider.h>

class Sphere : public Actor
{
public:

  Sphere(
    World& world,
    ActorProxy* proxy,
    std::string const& name)
    : Actor(world, proxy, name)
    , mTransform{ World::AttachComponent<Transform>(mWorld, this, R32V3{ 0.0f, 15.0f, 0.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mRenderable{ World::AttachComponent<Renderable>(mWorld, this, "Sphere", "CrossHatch") }
    , mCollider{ World::AttachComponent<SphereCollider>(mWorld, this, mTransform) }
    , mRigidbody{ World::AttachComponent<Rigidbody>(mWorld, this, 100.0f, mTransform, mCollider) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline SphereCollider* GetCollider() const { return mCollider; }
  inline Rigidbody* GetRigidbody() const { return mRigidbody; }

  inline bool TryToDestroy()
  {
    if (mTimeToLife > 0.0f)
    {
      mTimeToLife -= 0.002f;
      return false;
    }
    else
    {
      return true;
    }
  }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  SphereCollider* mCollider;
  Rigidbody* mRigidbody;

  R32 mTimeToLife = 2.0f;
};