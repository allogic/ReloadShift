#pragma once

#include <Core.h>
#include <CollisionComponent.h>

class Transform;

class SphereCollider : public CollisionComponent
{
public:

  SphereCollider(
    World& world,
    Transform* transform);
};