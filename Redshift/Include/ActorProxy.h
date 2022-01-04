#pragma once

#include <Core.h>
#include <Config.h>

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

  Actor* mActor = nullptr;

  void* mComponents[RS_MAX_COMPONENTS] = {};
};