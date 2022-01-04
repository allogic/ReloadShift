#pragma once

#include <Core.h>
#include <ActorProxy.h>

class World;

class Actor
{
public:

  friend ActorProxy;

public:

  Actor(World* world, ActorProxy* proxy)
    : mWorld{ world }
    , mProxy{ proxy }
  {

  }
  virtual ~Actor() = default;

public:

  inline ActorProxy* GetProxy() const { return mProxy; }
  inline std::string const& GetName() const { return mName; }
  inline U64 GetMask() const { return mMask; }

  template<typename C>
  inline C* GetComponent(U32 index) const { return (C*)mProxy->mComponents[index]; }

public:

  inline void SetMask(U64 value) { mMask = value; }
  inline void SetName(std::string const& value) { mName = value; }

public:

  inline void SetComponent(U32 index, void* value) { mProxy->mComponents[index] = value; }

private:

  World* mWorld;
  ActorProxy* mProxy;

  std::string mName = "";
  U64 mMask = 0;
};