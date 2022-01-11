#pragma once

#include <Redshift.h>

#include <Components/Brain.h>
#include <Components/Renderable.h>

class Enemy : public Actor
{
public:

  Enemy(
    World* world,
    ActorProxy* proxy,
    std::string const& name)
    : Actor(world, proxy, name)
    , mTransform{ world->AttachComponent<Transform>(this, R32V3{ 0.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mRenderable{ world->AttachComponent<Renderable>(this, "Cube", "Lit", "PukerAlbedo", "PukerNormal", "PukerSpecular", "PukerMetallic", "PukerRoughness")}
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