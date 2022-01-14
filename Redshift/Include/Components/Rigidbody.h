#pragma once

#include <Core.h>
#include <Component.h>

class Rigidbody : public Component
{
public:

  Rigidbody(
    World& world,
    R32 mass)
    : Component(world)
    , mMass{ mass }
  {

  }

private:

  R32 mMass;

  //btDefaultMotionState mMotionState;
  //btRigidBody::btRigidBodyConstructionInfo mConstructionInfo = btRigidBody::btRigidBodyConstructionInfo{ mMass, mMotionState, btVector3{} };
  //btRigidBody mRigidbody = btRigidBody{ mConstructionInfo };
};