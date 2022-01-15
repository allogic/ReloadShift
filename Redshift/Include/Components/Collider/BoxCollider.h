#pragma once

#include <Core.h>
#include <Component.h>

class Transform;

class BoxCollider : public Component
{
public:

  BoxCollider(
    World& world,
    Transform* transform);

public:

  inline btCollisionShape* GetShape() const { return (btCollisionShape*)&mShape; }

private:

  btBoxShape mShape;
};