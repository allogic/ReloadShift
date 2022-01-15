#pragma once

#include <Core.h>
#include <Component.h>

class Transform;

class SphereCollider : public Component
{
public:

  SphereCollider(
    World& world,
    Transform* transform);

public:

  inline btCollisionShape* GetShape() const { return (btCollisionShape*)&mShape; }

private:

  btSphereShape mShape;
};