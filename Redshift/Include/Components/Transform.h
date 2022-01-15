#pragma once

#include <Core.h>
#include <Component.h>

class Transform : public Component
{
public:

  Transform(
    World& world,
    R32V3 position,
    R32V3 rotation,
    R32V3 scale)
    : Component(world)
    , mPosition{ position }
    , mRotation{ rotation }
    , mScale{ scale }
    , mTransform{ btQuaternion{ mRotation.y, mRotation.x, mRotation.z }, btVector3{ mPosition.x, mPosition.y, mPosition.z } }
  {

  }

public:

  inline R32V3 const& GetWorldPosition() const { return mPosition; }
  inline R32V3 const& GetWorldRotation() const { return mRotation; }
  inline R32V3 const& GetWorldScale() const { return mScale; }

  inline R32V3 const& GetLocalRight() const { return mLocalRight; }
  inline R32V3 const& GetLocalUp() const { return mLocalUp; }
  inline R32V3 const& GetLocalForward() const { return mLocalForward; }

  inline R32M4 const& GetMatrix() const { return mMatrix; }

  inline btTransform const& GetTransform() const { return mTransform; }

public:

  void Update()
  {
    R32M4 matrix = glm::identity<R32M4>();

    // Translate
    matrix = glm::translate(matrix, mPosition);

    // Rotate
    matrix = glm::rotate(matrix, glm::radians(mRotation.x), R32V3{ 1.0f, 0.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::radians(mRotation.y), R32V3{ 0.0f, 1.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::radians(mRotation.z), R32V3{ 0.0f, 0.0f, 1.0f });
  
    // Compute new local directions
    mLocalRight = glm::normalize(R32V3{ matrix[0][0], matrix[1][0], matrix[2][0] });
    mLocalUp = glm::normalize(R32V3{ matrix[0][1], matrix[1][1], matrix[2][1] });
    mLocalForward = glm::normalize(R32V3{ matrix[0][2], matrix[1][2], matrix[2][2] });

    // Scale
    matrix = glm::scale(matrix, mScale);

    // Update physics transform
    mTransform.setIdentity();
    mTransform.setOrigin(btVector3{ mPosition.x, mPosition.y, mPosition.z });
    mTransform.setRotation(btQuaternion{ mRotation.y, mRotation.x, mRotation.z });

    mMatrix = matrix;
  }

public:

  inline void SetWorldPosition(R32V3 const& value) { mPosition = value; }
  inline void SetWorldRotation(R32V3 const& value) { mRotation = value; }
  inline void SetWorldScale(R32V3 const& value) { mScale = value; }

private:

  R32V3 mPosition;
  R32V3 mRotation;
  R32V3 mScale;
  btTransform mTransform;

  R32V3 mLocalRight = R32V3{ 1.0f, 0.0f, 0.0f };
  R32V3 mLocalUp = R32V3{ 0.0f, 1.0f, 0.0f };
  R32V3 mLocalForward = R32V3{ 0.0f, 0.0f, 1.0f };

  R32M4 mMatrix = glm::identity<R32M4>();
};