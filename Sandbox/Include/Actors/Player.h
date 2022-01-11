#pragma once

#include <Redshift.h>

class Player : public Actor
{
public:

  Player(
    World* world,
    ActorProxy* proxy,
    std::string const& name)
    : Actor(world, proxy, name)
    , mTransform{ world->AttachComponent<Transform>(this, R32V3{ 0.0f, 0.0f, -5.0f }, R32V3{ 0.0f }, R32V3{ 1.0f }) }
    , mCamera{ world->AttachComponent<Camera>(this, 45.0f, 0.001f, 1000.0f) }
  {

  }

public:

  inline Transform* GetTransform() const { return mTransform; }
  inline Renderable* GetRenderable() const { return mRenderable; }
  inline Camera* GetCamera() const { return mCamera; }

public:

  virtual void SetupInput(EventRegistry* eventRegistry) const
  {
    eventRegistry->BindAxis("MoveForward", this, &Player::MoveForward);
    eventRegistry->BindAxis("MoveRight", this, &Player::MoveRight);

    eventRegistry->BindAxis("TurnHorizontal", this, &Player::TurnHorizontal);
    eventRegistry->BindAxis("TurnVertical", this, &Player::TurnVertical);

    eventRegistry->BindAction("G", EInputType::Pressed, this, &Player::OnButtonPressed);
    eventRegistry->BindAction("G", EInputType::Held, this, &Player::OnButtonHeld);
    eventRegistry->BindAction("G", EInputType::Released, this, &Player::OnButtonReleased);
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

  void OnButtonPressed()
  {
    std::printf("G Pressed\n");
  }
  void OnButtonHeld()
  {
    std::printf("G Held\n");
  }
  void OnButtonReleased()
  {
    std::printf("G Released\n");
  }

private:

  Transform* mTransform;
  Renderable* mRenderable;
  Camera* mCamera;
};