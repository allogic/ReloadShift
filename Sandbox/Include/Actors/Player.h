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
    , mTransform{ World::AttachComponent<Transform>(world, this, R32V3{ 0.0f, 0.0f, -5.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mCamera{ World::AttachComponent<Camera>(world, this, 45.0f, 0.1f, 1000.0f) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline Camera* GetCamera() const { return mCamera; }

public:

  virtual void SetupInput(EventRegistry& eventRegistry) const
  {
    //eventRegistry.BindAxis("MoveForward", this, &Player::MoveForward);
    //eventRegistry.BindAxis("MoveRight", this, &Player::MoveRight);

    //eventRegistry.BindAxis("TurnHorizontal", this, &Player::TurnHorizontal);
    //eventRegistry.BindAxis("TurnVertical", this, &Player::TurnVertical);

    eventRegistry.BindAction('W', EInputType::Pressed, this, &Player::OnMoveForward);
    eventRegistry.BindAction('W', EInputType::Held, this, &Player::OnMoveForward);

    eventRegistry.BindAction('S', EInputType::Pressed, this, &Player::OnMoveBackward);
    eventRegistry.BindAction('S', EInputType::Held, this, &Player::OnMoveBackward);

    eventRegistry.BindAction('D', EInputType::Pressed, this, &Player::OnMoveRight);
    eventRegistry.BindAction('D', EInputType::Held, this, &Player::OnMoveRight);

    eventRegistry.BindAction('A', EInputType::Pressed, this, &Player::OnMoveLeft);
    eventRegistry.BindAction('A', EInputType::Held, this, &Player::OnMoveLeft);
  }

private:

  void MoveForward(float value)
  {
    std::printf("Forward: %f\n", value);
  }
  void MoveRight(float value)
  {
    std::printf("Right: %f\n", value);
  }

  void TurnHorizontal(float value)
  {
    std::printf("Horizontal: %f\n", value);
  }
  void TurnVertical(float value)
  {
    std::printf("Vertical: %f\n", value);
  }

  void OnMoveForward()
  {
    mTransform->AddWorldPosition(R32V3{ 0.0f, 0.0f, 1.0f } * 1.0f);
  }
  void OnMoveBackward()
  {
    mTransform->AddWorldPosition(R32V3{ 0.0f, 0.0f, -1.0f } * 1.0f);
  }
  void OnMoveRight()
  {
    mTransform->AddWorldPosition(R32V3{ -1.0f, 0.0f, 0.0f } * 1.0f);
  }
  void OnMoveLeft()
  {
    mTransform->AddWorldPosition(R32V3{ 1.0f, 0.0f, 0.0f } * 1.0f);
  }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  Camera* mCamera;
};