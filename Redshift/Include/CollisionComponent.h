#pragma once

#include <Core.h>
#include <Component.h>

class CollisionComponent : public Component
{
public:

  CollisionComponent(
    World& world,
    btCollisionShape* shape)
    : Component(world)
    , mShape{ shape }
  {

  }
  virtual ~CollisionComponent()
  {
    delete mShape; mShape = nullptr;
  }

public:

  inline btCollisionShape* GetShape() const { return mShape; }

private:

  btCollisionShape* mShape;
};