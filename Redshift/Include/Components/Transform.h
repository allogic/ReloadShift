#pragma once

#include <Core.h>
#include <Component.h>

class Transform : public Component
{
public:

  Transform(
    World* world,
    R32V3 position,
    R32V3 rotation,
    R32V3 scale)
    : Component(world)
    , mPosition{ position }
    , mRotation{ rotation }
    , mScale{ scale }
  {

  }

public:

  inline R32V3 const& GetPosition() const { return mPosition; }
  inline R32V3 const& GetRotation() const { return mRotation; }
  inline R32V3 const& GetScale() const { return mScale; }

public:

  inline R32M4 GetMatrix() const
  {
    R32M4 matrix = glm::identity<R32M4>();
    matrix = glm::translate(matrix, mPosition);
    matrix = glm::rotate(matrix, glm::degrees(mRotation.x), R32V3{ 1.0f, 0.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::degrees(mRotation.y), R32V3{ 0.0f, 1.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::degrees(mRotation.z), R32V3{ 0.0f, 0.0f, 1.0f });
    matrix = glm::scale(matrix, mScale);
    return matrix;
  }

public:

  inline void SetPosition(R32V3 const& value) { mPosition = value; }
  inline void SetRotation(R32V3 const& value) { mRotation = value; }
  inline void SetScale(R32V3 const& value) { mScale = value; }

private:

  R32V3 mPosition;
  R32V3 mRotation;
  R32V3 mScale;
};