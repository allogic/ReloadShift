#pragma once

#include <Redshift.h>

class Player : public Actor
{
public:

  Player(
    World& world,
    ActorProxy* proxy,
    std::string const& name)
    : Actor(world, proxy, name)
    , mTransform{ World::AttachComponent<Transform>(world, this, R32V3{ 0.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mCamera{ World::AttachComponent<Camera>(world, this, 45.0f, 0.1f, 10000.0f) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline Camera* GetCamera() const { return mCamera; }

public:

  virtual void SetupInput(EventRegistry& eventRegistry)
  {
    eventRegistry.BindAxis("Horizontal", this, &Player::TurnHorizontal);
    eventRegistry.BindAxis("Vertical", this, &Player::TurnVertical);

    eventRegistry.BindAction(GLFW_MOUSE_BUTTON_RIGHT, EInputState::Pressed, this, &Player::OnMouseButtonRightPressed);
    eventRegistry.BindAction(GLFW_MOUSE_BUTTON_RIGHT, EInputState::Released, this, &Player::OnMouseButtonRightReleased);

    eventRegistry.BindAction(GLFW_KEY_W, EInputState::Held, this, &Player::OnMoveForward);
    eventRegistry.BindAction(GLFW_KEY_S, EInputState::Held, this, &Player::OnMoveBackward);
    eventRegistry.BindAction(GLFW_KEY_A, EInputState::Held, this, &Player::OnMoveLeft);
    eventRegistry.BindAction(GLFW_KEY_D, EInputState::Held, this, &Player::OnMoveRight);
    eventRegistry.BindAction(GLFW_KEY_Q, EInputState::Held, this, &Player::OnMoveUp);
    eventRegistry.BindAction(GLFW_KEY_E, EInputState::Held, this, &Player::OnMoveDown);
  }

private:

  void TurnHorizontal(R32 value)
  {
    if (mEnableRotation)
    {
      R32V3 worldRotation = mTransform->GetWorldRotation();
      if (worldRotation.y > 360.0f) worldRotation.y = 0.0f;
      if (worldRotation.y < 0.0f) worldRotation.y = 360.0f;
      mTransform->SetWorldRotation(worldRotation + R32V3{ 0.0f, 1.0f, 0.0f } * value * 0.2f);
    }
  }
  void TurnVertical(R32 value)
  {
    if (mEnableRotation)
    {
      R32V3 worldRotation = mTransform->GetWorldRotation();
      if (worldRotation.x > 90.0f) worldRotation.x = 90.0f;
      if (worldRotation.x < -90.0f) worldRotation.x = -90.0f;
      mTransform->SetWorldRotation(worldRotation + R32V3{ 1.0f, 0.0f, 0.0f } * value * 0.3f);
    }
  }

  void OnMouseButtonRightPressed()
  {
    mEnableRotation = true;
  }
  void OnMouseButtonRightReleased()
  {
    mEnableRotation = false;
  }

  void OnMoveForward()
  {
    R32V3 worldPosition = mTransform->GetWorldPosition();
    mTransform->SetWorldPosition(worldPosition + mTransform->GetLocalForward() * 0.02f);
  }
  void OnMoveBackward()
  {
    R32V3 worldPosition = mTransform->GetWorldPosition();
    mTransform->SetWorldPosition(worldPosition + -mTransform->GetLocalForward() * 0.02f);
  }
  void OnMoveLeft()
  {
    R32V3 worldPosition = mTransform->GetWorldPosition();
    mTransform->SetWorldPosition(worldPosition + mTransform->GetLocalRight() * 0.02f);
  }
  void OnMoveRight()
  {
    R32V3 worldPosition = mTransform->GetWorldPosition();
    mTransform->SetWorldPosition(worldPosition + -mTransform->GetLocalRight() * 0.02f);
  }
  void OnMoveUp()
  {
    R32V3 worldPosition = mTransform->GetWorldPosition();
    mTransform->SetWorldPosition(worldPosition + mTransform->GetLocalUp() * 0.02f);
  }
  void OnMoveDown()
  {
    R32V3 worldPosition = mTransform->GetWorldPosition();
    mTransform->SetWorldPosition(worldPosition + -mTransform->GetLocalUp() * 0.02f);
  }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  Camera* mCamera;

  bool mEnableRotation = false;
};