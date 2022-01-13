#pragma once

#include <Core.h>
#include <Component.h>

class Actor;

class ActorProxy
{
public:

  friend Actor;

public:

  ActorProxy() = default;
  virtual ~ActorProxy() = default;

public:

  template<typename A = Actor>
  inline A* GetActor() const { return (A*)mActor; }

public:

  inline void SetActor(Actor* value) { mActor = value; }

public:

  void DestroyAllComponents()
  {
    for (auto const& [hash, component] : mComponents)
    {
      delete component;
    }
    mComponents.clear();
  }

private:

  Actor* mActor = nullptr;

  std::map<U64, Component*> mComponents = {};
  std::vector<U64> mInOrderComponentHashes = {};
  U64 mCurrentHash = 0;
  U32 mComponentCount = 0;
};