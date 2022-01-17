#pragma once

#include <Core.h>
#include <Component.h>

class LightComponent : public Component
{
public:

  LightComponent(
    World& world,
    R32 intensity,
    R32V3 const& color,
    bool enabled)
    : Component(world)
    , mIntensity{ intensity }
    , mColor{ color }
    , mEnabled{ enabled }
  {

  }

public:

  inline R32 GetIntensity() const { return mIntensity; }
  inline R32V3 const& GetColor() const { return mColor; }
  inline bool GetEnabled() const { return mEnabled; }

  inline R32& GetIntensity() { return mIntensity; }
  inline R32V3& GetColor() { return mColor; }
  inline bool& GetEnabled() { return mEnabled; }

private:

  R32 mIntensity;
  R32V3 mColor;
  bool mEnabled;
};