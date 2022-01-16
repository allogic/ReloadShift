#pragma once

#include <Core.h>
#include <CollisionComponent.h>

class Transform;

class BoxCollider : public CollisionComponent
{
public:

  BoxCollider(
    World& world,
    Transform* transform);
};