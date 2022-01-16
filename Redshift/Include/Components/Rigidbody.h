#pragma once

#include <Core.h>
#include <Component.h>

class Transform;
class CollisionComponent;

class Rigidbody : public Component
{
public:

  Rigidbody(
    World& world,
    R32 mass,
    Transform* transform,
    CollisionComponent* collider);
  virtual ~Rigidbody();

public:

  inline btRigidBody* GetBody() { return &mRigidbody; }

private:

  btDefaultMotionState mMotionState;
  btRigidBody::btRigidBodyConstructionInfo mRigidbodyInfo;
  btRigidBody mRigidbody = btRigidBody{ mRigidbodyInfo };
};