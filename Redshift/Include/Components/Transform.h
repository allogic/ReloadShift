#pragma once

#include <Core.h>
#include <Component.h>

class Transform : public Component
{
public:

  Transform(
    World& world,
    R32V3 worldPosition,
    R32V3 worldRotation,
    R32V3 worldScale)
    : Component(world)
  {
    SetWorldPosition(worldPosition);
    SetWorldRotation(worldRotation);
    SetWorldScale(worldScale);
  }

public:

  inline R32V3 GetWorldPosition() const { return R32V3{ mTransform.getOrigin().x(), mTransform.getOrigin().y(), mTransform.getOrigin().z() }; }
  inline R32V3 GetWorldRotation() const { return R32V3{ mTransform.getRotation().x(), mTransform.getRotation().y(), mTransform.getRotation().z() }; }
  inline R32V3 GetWorldScale() const { return mScale; }

public:

  inline R32M4 GetModelMatrix() const
  {
    R32M4 m = glm::identity<R32M4>();

    // Translation
    btVector3 const& o = mTransform.getOrigin();
    m[3][0] = o.getX();
    m[3][1] = o.getY();
    m[3][2] = o.getZ();

    // Rotation
    btMatrix3x3 const& b = mTransform.getBasis();
    m[0][0] = b[0][0];
    m[1][0] = b[0][1];
    m[2][0] = b[0][2];
    
    m[0][1] = b[1][0];
    m[1][1] = b[1][1];
    m[2][1] = b[1][2];
    
    m[0][2] = b[2][0];
    m[1][2] = b[2][1];
    m[2][2] = b[2][2];

    // Scale
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;

    //matrix = glm::translate(matrix, mWorldPosition);
    //matrix = glm::rotate(matrix, glm::degrees(mWorldRotation.x), R32V3{ 1.0f, 0.0f, 0.0f });
    //matrix = glm::rotate(matrix, glm::degrees(mWorldRotation.y), R32V3{ 0.0f, 1.0f, 0.0f });
    //matrix = glm::rotate(matrix, glm::degrees(mWorldRotation.z), R32V3{ 0.0f, 0.0f, 1.0f });
    //matrix = glm::scale(matrix, mScale);

    return m;
  }

public:

  inline void SetWorldPosition(R32V3 const& value) { mTransform.setOrigin(btVector3{ value.x, value.y, value.z }); }
  inline void SetWorldRotation(R32V3 const& value) { mTransform.setRotation(btQuaternion{ value.x, value.y, value.z }); }
  inline void SetWorldScale(R32V3 const& value) { mScale = value; }

  inline void AddWorldPosition(R32V3 const& value) { mTransform.setOrigin(mTransform.getOrigin() + btVector3{ value.x, value.y, value.z }); }
  inline void AddWorldRotation(R32V3 const& value) { mTransform.setRotation(mTransform.getRotation() + btQuaternion{ value.x, value.y, value.z }); }
  inline void AddWorldScale(R32V3 const& value) { mScale += value; }

private:

  btTransform mTransform;
  R32V3 mScale;
};