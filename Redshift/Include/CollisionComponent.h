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

  inline btCollisionShape const* GetShape() const { return mShape; }

  inline btCollisionShape* GetShape() { return mShape; }

private:

  btCollisionShape* mShape;
};