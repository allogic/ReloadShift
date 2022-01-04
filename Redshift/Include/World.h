#pragma once

#include <Core.h>
#include <Actor.h>
#include <Handle.h>
#include <ModuleProxy.h>
#include <Utility.h>
#include <Component.h>

class HotLoader;
class Resource;
class ActorProxy;

class World
{
public:

  ////////////////////////////////////////////////////////
  // Singleton
  ////////////////////////////////////////////////////////

  static inline World& Instance() { static World world; return world; }

public:

  World();

public:

  ////////////////////////////////////////////////////////
  // Getter
  ////////////////////////////////////////////////////////

  inline GladGLContext* GetGladContext() const { return mGladContext; }
  inline ImGuiContext* GetImGuiContext() const { return mImGuiContext; }

  inline std::map<std::string, ModuleProxy*>& GetModules() { return mModules; }
  inline std::map<std::string, Resource*>& GetResources() { return mResources; }
  inline std::multimap<std::string, Handle*>& GetHandles() { return mHandles; }
  inline std::multimap<U64, ActorProxy*>& GetActors() { return mActors; }

public:

  ////////////////////////////////////////////////////////
  // Resource interface
  ////////////////////////////////////////////////////////

  template<typename R, typename ... Args>
  requires std::is_base_of_v<Resource, R>
  R* MountResource(std::string const& resourceName, Args &&... args)
  {
    // Compute keys
    std::string resourceKey = std::string{ typeid(R).name() } + ':' + resourceName;
    // Check if resource exists, if not, insert new resource
    auto const resourceIt = mResources.find(resourceKey);
    if (resourceIt == mResources.end())
    {
      auto const& [emplaceIt, _] = mResources.emplace(resourceKey, new R{ this, resourceName, std::forward<Args>(args) ... });
      // Enable auto load file and produce handles
      if (emplaceIt->second->LoadFile())
      {
        emplaceIt->second->ProduceHandles();
      }
      return (R*)emplaceIt->second;
    }
    return (R*)resourceIt->second;
  }

  template<typename R>
  requires std::is_base_of_v<Resource, R>
  void UnMountResource(std::string const& resourceName)
  {
    // Compute keys
    std::string resourceKey = std::string{ typeid(R).name() } + ':' + resourceName;
    // Destroy resource
    auto const resourceIt = mResources.find(resourceKey);
    if (resourceIt != mResources.end())
    {
      delete resourceIt->second;
      mResources.erase(resourceIt);
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Handle utilities
  ////////////////////////////////////////////////////////

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  void CollectDirtyHandleNamesByType(std::set<std::string>& handleNames)
  {
    for (auto it = mHandles.begin(); it != mHandles.end(); ++it)
    {
      if (it->second->GetType() == typeid(H).name())
      {
        if (it->second->GetDirty())
        {
          handleNames.emplace(it->second->GetName());
        }
      }
    }
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  void CollectNonDirtyHandleNamesByType(std::set<std::string>& handleNames)
  {
    for (auto it = mHandles.begin(); it != mHandles.end(); ++it)
    {
      if (it->second->GetType() == typeid(H).name())
      {
        if (!it->second->GetDirty())
        {
          handleNames.emplace(it->second->GetName());
        }
      }
    }
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  H* GetFirstDirtyHandleByName(std::string const& name)
  {
    std::string handleKey = std::string{ typeid(H).name() } + ':' + name;
    auto const handleIt = mHandles.equal_range(handleKey);
    for (auto it = handleIt.first; it != handleIt.second; ++it)
    {
      if (it->second->GetDirty())
      {
        return (H*)it->second;
      }
    }
    return nullptr;
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  H* GetFirstNonDirtyHandleByName(std::string const& name)
  {
    std::string handleKey = std::string{ typeid(H).name() } + ':' + name;
    auto const handleIt = mHandles.equal_range(handleKey);
    for (auto it = handleIt.first; it != handleIt.second; ++it)
    {
      if (!it->second->GetDirty())
      {
        return (H*)it->second;
      }
    }
    return nullptr;
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  void MarkHandlesAsDirtyByName(std::string const& name)
  {
    std::string handleKey = std::string{ typeid(H).name() } + ':' + name;
    auto const handleIt = mHandles.equal_range(handleKey);
    for (auto it = handleIt.first; it != handleIt.second; ++it)
    {
      it->second->SetDirty(true);
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Handle interface
  ////////////////////////////////////////////////////////

  template<typename H, typename ... Args>
  requires std::is_base_of_v<Handle, H>
  H* CreateHandle(std::string const& handleName, Args &&... args)
  {
    // Compute keys
    std::string handleKey = std::string{ typeid(H).name() } + ':' + handleName;
    // Mark all existing handles as dirty
    auto const handleIt = mHandles.equal_range(handleKey);
    for (auto it = handleIt.first; it != handleIt.second; ++it)
    {
      it->second->SetDirty(true);
    }
    // Insert new handle
    auto const& emplaceIt = mHandles.emplace(handleKey, new H{ handleName, std::forward<Args>(args) ... });
    emplaceIt->second->Create();
    // Resolve doubled entries
    return (H*)emplaceIt->second;
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  void DestroyHandle(std::string const& handleName)
  {
    // Compute keys
    std::string handleKey = std::string{ typeid(H).name() } + ':' + handleName;
    // Erase and destroy handle
    for (auto it = mHandles.begin(); it != mHandles.end(); ++it)
    {
      if (it->second->GetType() == typeid(H).name() && it->second->GetName() == handleName)
      {
        it->second->Destroy();
        delete it->second;
        it = mHandles.erase(it);
      }
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Actor/Component internal
  ////////////////////////////////////////////////////////

  template<typename C>
  requires std::is_base_of_v<Component, C>
  U32 ComponentToIndex()
  {
    U64 hash = typeid(C).hash_code();
    if (mIdentities[hash] == 0)
    {
      mIdentities[hash] = mUniqueComponentCount++;
    }
    return mIdentities[hash];
  }

  U32 ContainsComponentBits(U64 currentMask, U64 desiredMask)
  {
    return (~(currentMask ^ desiredMask) & desiredMask) == desiredMask;
  }

  void MergeWithNewMask(ActorProxy* actorProxy, Actor* actor, U64 desiredMask)
  {
    auto const actorIt = mActors.equal_range(actor->GetMask());
    for (auto it = actorIt.first; it != actorIt.second; ++it)
    {
      if (actorProxy == it->second)
      {
        mActors.erase(it);
        actor->SetMask(desiredMask);
        mActors.emplace(desiredMask, actorProxy);
        break;
      }
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Actor/Component interface
  ////////////////////////////////////////////////////////

  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  C* Attach(Actor* actor, Args && ... args)
  {
    U32 componentIndex = ComponentToIndex<C>();
    U64 componentMask = (U64)1 << componentIndex;
    if (ContainsComponentBits(actor->GetMask(), componentMask))
    {
      return actor->GetComponent<C>(componentIndex);
    }
    else
    {
      U64 newMask = actor->GetMask() | componentMask;
      MergeWithNewMask(actor->GetProxy(), actor, newMask);
      actor->SetComponent(componentIndex, new C{ this, std::forward<Args>(args) ... });
      return actor->GetComponent<C>(componentIndex);
    }
  }

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A* Create(std::string const& actorName, Args && ... args)
  {
    ActorProxy* proxy = new ActorProxy{};
    mActors.emplace(0u, proxy);
    Actor* actor = new A{ this, proxy, std::forward<Args>(args) ... };
    actor->SetName(actorName);
    proxy->SetActor(actor);
    return proxy->GetActor<A>();
  }

  template<typename ... Cs>
  void DispatchFor(std::function<void(typename TypeProxy<Cs>::Ptr ...)>&& predicate)
  {
    U64 componentMask = (((U64)1 << ComponentToIndex<typename TypeProxy<Cs>::Type>()) | ... | (U64)0);
    auto const actorIt = mActors.equal_range(componentMask);
    for (auto it = actorIt.first; it != actorIt.second; ++it)
    {
      predicate
      (
        (it->second->GetActor()->GetComponent<typename TypeProxy<Cs>::Type>(ComponentToIndex<typename TypeProxy<Cs>::Type>()))
        ...
      );
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Module interface
  ////////////////////////////////////////////////////////

  bool CreateModule(std::filesystem::path const& filePath);
  bool DestroyModule(std::string const& moduleName);

private:

  ////////////////////////////////////////////////////////
  // State
  ////////////////////////////////////////////////////////

  GladGLContext* mGladContext = nullptr;
  ImGuiContext* mImGuiContext = nullptr;

  std::map<std::string, ModuleProxy*> mModules = {};
  std::map<std::string, Resource*> mResources = {};
  std::multimap<std::string, Handle*> mHandles = {};
  std::multimap<U64, ActorProxy*> mActors = {};

  U32 mUniqueComponentCount = 0;
  std::map<U64, U32> mIdentities = {};
};