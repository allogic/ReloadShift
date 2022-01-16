#include <Components/Rigidbody.h>

#include <Components/Transform.h>
#include <Components/Collider/BoxCollider.h>
#include <Components/Collider/SphereCollider.h>

#include <Globals/World.h>

#include <CollisionComponent.h>

Rigidbody::Rigidbody(
  World& world,
  R32 mass,
  Transform* transform,
  CollisionComponent* collider)
  : Component(world)
  , mMotionState{ transform->GetTransform() }
  , mRigidbodyInfo{ mass, &mMotionState, collider->GetShape(), btVector3{ 0.0f, 0.0f, 0.0f } }
{
  World::CreatePhysicsBody(mWorld, this);
}
Rigidbody::~Rigidbody()
{
  World::DestroyPhysicsBody(mWorld, this);
}