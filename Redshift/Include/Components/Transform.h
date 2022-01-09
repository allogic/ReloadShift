#pragma once

#include <Core.h>
#include <Component.h>

class Transform : public Component
{
public:

  Transform(
    World* world,
    R32V3 worldPosition,
    R32V3 worldRotation,
    R32V3 worldScale)
    : Component(world)
    , mWorldPosition{ worldPosition }
    , mWorldRotation{ worldRotation }
    , mWorldScale{ worldScale }
  {

  }

public:

  inline R32V3 const& GetWorldPosition() const { return mWorldPosition; }
  inline R32V3 const& GetWorldRotation() const { return mWorldRotation; }
  inline R32V3 const& GetWorldScale() const { return mWorldScale; }

  inline R32V3 const& GetRelativePosition() const { return mRelativePosition; }
  inline R32V3 const& GetRelativeRotation() const { return mRelativeRotation; }
  inline R32V3 const& GetRelativeScale() const { return mRelativeScale; }

public:

  inline R32M4 GetMatrix() const
  {
    R32M4 matrix = glm::identity<R32M4>();
    // TRS in world space
    matrix = glm::translate(matrix, mWorldPosition);
    matrix = glm::rotate(matrix, glm::degrees(mWorldRotation.x), R32V3{ 1.0f, 0.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::degrees(mWorldRotation.y), R32V3{ 0.0f, 1.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::degrees(mWorldRotation.z), R32V3{ 0.0f, 0.0f, 1.0f });
    matrix = glm::scale(matrix, mWorldScale);
    // TRS in relative space
    matrix = glm::translate(matrix, mRelativePosition);
    matrix = glm::rotate(matrix, glm::degrees(mRelativeRotation.x), R32V3{ 1.0f, 0.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::degrees(mRelativeRotation.y), R32V3{ 0.0f, 1.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::degrees(mRelativeRotation.z), R32V3{ 0.0f, 0.0f, 1.0f });
    matrix = glm::scale(matrix, mRelativeScale);
    return matrix;
  }

public:

  inline void SetWorldPosition(R32V3 const& value) { mWorldPosition = value; }
  inline void SetWorldRotation(R32V3 const& value) { mWorldRotation = value; }
  inline void SetWorldScale(R32V3 const& value) { mWorldScale = value; }

  inline void SetRelativePosition(R32V3 const& value) { mRelativePosition = value; }
  inline void SetRelativeRotation(R32V3 const& value) { mRelativeRotation = value; }
  inline void SetRelativeScale(R32V3 const& value) { mRelativeScale = value; }

private:

  R32V3 mWorldPosition;
  R32V3 mWorldRotation;
  R32V3 mWorldScale;

  R32V3 mRelativePosition = R32V3{ 0.0f };
  R32V3 mRelativeRotation = R32V3{ 0.0f };
  R32V3 mRelativeScale = R32V3{ 1.0f };
};