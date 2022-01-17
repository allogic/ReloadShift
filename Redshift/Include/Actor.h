#pragma once

#include <Core.h>
#include <ActorProxy.h>

class Delegates;
class Component;
class World;

class Actor
{
public:

  friend ActorProxy;

public:

  Actor(World& world, ActorProxy* proxy, std::string const& name);
  virtual ~Actor() = default;

public:

  inline bool GetSelected() const { return mSelected; }
  inline ActorProxy* GetProxy() const { return mProxy; }
  inline std::string const& GetName() const { return mName; }
  inline U64 GetCurrentHash() const { return mProxy->mCurrentHash; }
  inline U32 GetComponentCount() const { return mProxy->mComponentCount; }
  inline std::vector<U64> const& GetInOrderComponentHashes() const { return mProxy->mInOrderComponentHashes; }

public:

  inline void SetSelected(bool value) { mSelected = value; }

public:

  template<typename C>
  requires std::is_base_of_v<Component, C>
  inline C* GetComponent() const { return (C*)mProxy->mComponents[typeid(C).hash_code()]; }

  template<typename C>
  requires std::is_base_of_v<Component, C>
  inline C* CreateComponent(Component* value)
  {
    U64 componentHash = typeid(C).hash_code();
    mProxy->mInOrderComponentHashes.emplace_back(componentHash);
    mProxy->mCurrentHash ^= componentHash;
    mProxy->mComponentCount++;
    mProxy->mComponents[componentHash] = value;
    return (C*)value;
  }

public:

  virtual void SetupInput(Delegates& delegates) {}

protected:

  World& mWorld;

private:

  ActorProxy* mProxy;

  std::string mName = "";

private:

  bool mSelected = false;
};