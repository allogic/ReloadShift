#pragma once

#include <Redshift.h>

#include <Components/Brain.h>

class Enemy : public Actor
{
public:

  Enemy(
    World* world,
    ActorProxy* proxy,
    std::string const& name)
    : Actor(world, proxy, name)
    , mTransform{ world->AttachComponent<Transform>(this, R32V3{ 0.0f }, R32V3{ 0.0f }, R32V3{ 0.03f }) }
    , mRenderable{ world->AttachComponent<Renderable>(this, "Puker", "Lit") }
    , mBrain{ world->AttachComponent<Brain>(this) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline Brain* GetBrain() const { return mBrain; }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  Brain* mBrain;
};