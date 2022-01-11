#pragma once

#include <Core.h>
#include <ActorProxy.h>
#include <Component.h>

#include <Globals/EventRegistry.h>

class World;

class Actor
{
public:

  friend ActorProxy;

public:

  Actor(
    World* world,
    ActorProxy* proxy,
    std::string const& name)
    : mWorld{ world }
    , mProxy{ proxy }
    , mName{ name }
  {

  }
  virtual ~Actor() = default;

public:

  inline ActorProxy* GetProxy() const { return mProxy; }
  inline std::string const& GetName() const { return mName; }
  inline U64 GetCurrentHash() const { return mCurrentHash; }
  inline U32 GetComponentCount() const { return mProxy->mComponentCount; }
  inline std::vector<U64> const& GetInOrderComponentHashes() const { return mInOrderComponentHashes; }

public:

  template<typename C>
  requires std::is_base_of_v<Component, C>
  inline C* GetComponent(U64 hash) const { return (C*)mProxy->mComponents[hash]; }

  template<typename C>
  requires std::is_base_of_v<Component, C>
  inline C* CreateComponent(Component* value)
  {
    U64 componentHash = typeid(C).hash_code();
    mInOrderComponentHashes.emplace_back(componentHash);
    mCurrentHash ^= componentHash;
    mProxy->mComponentCount++;
    mProxy->mComponents[componentHash] = value;
    return (C*)value;
  }

public:

  virtual void SetupInput(EventRegistry* eventRegistry) const {}

private:

  World* mWorld;
  ActorProxy* mProxy;

  std::string mName = "";

  std::vector<U64> mInOrderComponentHashes = {};
  U64 mCurrentHash = 0;
};