#pragma once

#include <Core.h>

class Actor;
class Component;

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

private:

  Actor* mActor = nullptr;
  std::unordered_map<U64, Component*> mComponents = {};
  U32 mComponentCount = 0;
};