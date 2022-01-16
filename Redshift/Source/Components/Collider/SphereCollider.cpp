#include <Components/Collider/SphereCollider.h>
#include <Components/Transform.h>

SphereCollider::SphereCollider(
  World& world,
  Transform* transform)
  : CollisionComponent(
    world,
    new btSphereShape{ glm::length(transform->GetWorldScale() / 2.0f) })
{

}