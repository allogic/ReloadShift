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

  inline R32V3 GetLocalRight() const { return mLocalRight; }
  inline R32V3 GetLocalUp() const { return mLocalUp; }
  inline R32V3 GetLocalForward() const { return mLocalForward; }

public:

  R32M4 ComputeModelMatrix()
  {
    // Translation
    btVector3 const& o = mTransform.getOrigin();
    mMatrix[3][0] = o.getX();
    mMatrix[3][1] = o.getY();
    mMatrix[3][2] = o.getZ();

    // Rotation
    btMatrix3x3 const& b = mTransform.getBasis();
    mMatrix[0][0] = b[0][0];
    mMatrix[0][1] = b[0][1];
    mMatrix[0][2] = b[0][2];
    
    mMatrix[1][0] = b[1][0];
    mMatrix[1][1] = b[1][1];
    mMatrix[1][2] = b[1][2];
    
    mMatrix[2][0] = b[2][0];
    mMatrix[2][1] = b[2][1];
    mMatrix[2][2] = b[2][2];

    // Scale
    mMatrix[0] *= mScale.x;
    mMatrix[1] *= mScale.y;
    mMatrix[2] *= mScale.z;

    // Extract local directions
    mLocalRight =   R32V3{ mMatrix[0][0], mMatrix[0][1], mMatrix[0][2] };
    mLocalUp =      R32V3{ mMatrix[1][0], mMatrix[1][1], mMatrix[1][2] };
    mLocalForward = R32V3{ mMatrix[2][0], mMatrix[2][1], mMatrix[2][2] };

    return mMatrix;
  }

public:

  inline void SetWorldPosition(R32V3 const& value) { mTransform.setOrigin(btVector3{ value.x, value.y, value.z }); }
  inline void SetWorldRotation(R32V3 const& value) { mTransform.setRotation(btQuaternion{ value.x, value.y, value.z }); }
  inline void SetWorldScale(R32V3 const& value) { mScale = value; }

private:

  R32M4 mMatrix = glm::identity<R32M4>();

  R32V3 mScale;

  R32V3 mLocalRight;
  R32V3 mLocalUp;
  R32V3 mLocalForward;

  btTransform mTransform;
};