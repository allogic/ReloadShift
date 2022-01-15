#pragma once

#include <Core.h>
#include <Component.h>

class Transform;
class BoxCollider;
class SphereCollider;

class Rigidbody : public Component
{
public:

  Rigidbody(
    World& world,
    R32 mass,
    Transform* transform,
    BoxCollider* collider);
  Rigidbody(
    World& world,
    R32 mass,
    Transform* transform,
    SphereCollider* collider);
  virtual ~Rigidbody();

public:

  inline btRigidBody* GetBody() { return &mRigidbody; }

private:

  btDefaultMotionState mMotionState;
  btRigidBody::btRigidBodyConstructionInfo mRigidbodyInfo;
  btRigidBody mRigidbody = btRigidBody{ mRigidbodyInfo };
};