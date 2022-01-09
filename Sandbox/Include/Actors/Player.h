#pragma once

#include <Redshift.h>

class Player : public Actor
{
public:

  Player(
    World* world,
    ActorProxy* proxy,
    std::string const& name)
    : Actor(world, proxy, name)
    , mTransform{ world->AttachComponent<Transform>(this, R32V3{ 0.0f, 0.0f, -5.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mCamera{ world->AttachComponent<Camera>(this, 45.0f, 0.001f, 1000.0f) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline Camera* GetCamera() const { return mCamera; }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  Camera* mCamera;
};