#include <Components/Collider/BoxCollider.h>
#include <Components/Transform.h>

BoxCollider::BoxCollider(
  World& world,
  Transform* transform)
  : CollisionComponent(
    world,
    new btBoxShape{ btVector3{ transform->GetWorldScale().x, transform->GetWorldScale().y, transform->GetWorldScale().z } })
{

}