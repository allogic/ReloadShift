#pragma once

#include <Core.h>
#include <Actor.h>
#include <Handle.h>
#include <ModuleProxy.h>
#include <HotRef.h>
#include <Utility.h>
#include <Component.h>

class HotLoader;
class Resource;
class ActorProxy;

class World
{
public:

  ////////////////////////////////////////////////////////
  // Primitives
  ////////////////////////////////////////////////////////

  using ModulesByName = std::unordered_map<std::string, ModuleProxy*>;
  using ResourcesByName = std::unordered_map<std::string, Resource*>;
  using ActorsByName = std::unordered_multimap<std::string, ActorProxy*>;

  using HandlesByName = std::unordered_map<std::string, HotRef<Handle>>;
  using HandlesByNameByType = std::unordered_map<U64, HandlesByName>;

  using PermutationTable = std::unordered_map<U64, std::set<ActorProxy*>>;

public:

  ////////////////////////////////////////////////////////
  // Singleton
  ////////////////////////////////////////////////////////

  static inline World& Instance() { static World world; return world; }

public:

  ////////////////////////////////////////////////////////
  // Constructor
  ////////////////////////////////////////////////////////

  World();

public:

  ////////////////////////////////////////////////////////
  // Getter
  ////////////////////////////////////////////////////////

  inline GladGLContext* GetGladContext() const { return mGladContext; }
  inline ImGuiContext* GetImGuiContext() const { return mImGuiContext; }

  inline ModulesByName& GetModules() { return mModules; }
  inline ResourcesByName& GetResources() { return mResources; }
  inline ActorsByName GetActors() { return mActors; }

  inline HandlesByNameByType& GetHandles() { return mHandles; }

  inline PermutationTable& GetPermutationTable() { return mPermutationTable; }

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
  void ConsumeDirtyHandleNames(std::set<std::string>& handleNames) const noexcept
  {
    for (auto const& [name, hotRef] : mHandles[typeid(H).hash_code()])
    {
      if (hotRef.Get() && hotRef.Get()->GetDirty())
      {
        handleNames.emplace(name);
      }
    }
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  void ConsumeNonDirtyHandleNames(std::set<std::string>& handleNames) const noexcept
  {
    for (auto const& [name, hotRef] : mHandles[typeid(H).hash_code()])
    {
      if (hotRef.Get() && !hotRef.Get()->GetDirty())
      {
        handleNames.emplace(name);
      }
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Handle interface
  ////////////////////////////////////////////////////////

  template<typename H, typename ... Args>
  requires std::is_base_of_v<Handle, H>
  HotRef<H> const& LinkHandle(std::string const& handleName, Args &&... args)
  {
    HotRef<H>& hotRef = (HotRef<H>&)mHandles[typeid(H).hash_code()][handleName];
    // Probe if reference is valid
    if (!hotRef.Get())
    {
      // Create new handle
      hotRef.Set(new H{ handleName, std::forward<Args>(args) ... });
    }
    return hotRef;
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  H* GetHandleUnsafe(std::string const& handleName)
  {
    return (H*)mHandles[typeid(H).hash_code()][handleName].Get();
  }

public:

  ////////////////////////////////////////////////////////
  // Actor/Component interface
  ////////////////////////////////////////////////////////

  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  C* AttachComponent(Actor* actor, Args && ... args)
  {
    // Check if component already exists
    C* component = actor->GetComponent<C>(typeid(C).hash_code());
    if (!component)
    {
      // Update actor
      component = actor->CreateComponent<C>(new C{ this, std::forward<Args>(args) ... });
      // Register proxy for all single types
      auto permutations = actor->GetInOrderComponentHashes();
      for (auto const& typHash : permutations)
      {
        mPermutationTable[typHash].emplace(actor->GetProxy());
      }
      // Complete transaction by computing all type permutations
      // and insert the proxy in their respective buckets
      do
      {
        // Sequencially compute hashes from current permutation
        for (U32 i = 0; i < permutations.size(); ++i)
        {
          U64 permutationHash = 0;
          for (U32 j = i; j < permutations.size(); ++j)
          {
            permutationHash ^= permutations[j];
            mPermutationTable[permutationHash].emplace(actor->GetProxy());
          }
        }
      } while (std::next_permutation(permutations.begin(), permutations.end()));
    }
    return component;
  }

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A* CreateActor(std::string const& actorName, Args && ... args)
  {
    // Create actor proxy
    ActorProxy* proxy = new ActorProxy{};
    mActors.emplace(actorName, proxy);
    // Create actor after proxy in order to initialize components inside the constructor
    A* actor = new A{ this, proxy, actorName, std::forward<Args>(args) ... };
    proxy->SetActor(actor);
    return actor;
  }

  //template<typename A>
  //requires std::is_base_of_v<Actor, A>
  //void DestroyActor(std::vector<A*> const& actors)
  //{
  //  for (auto actor : actors)
  //  {
  //    auto it = mActors.begin();
  //    while (it != mActors.end())
  //    {
  //      if (actor == it->second->GetActor())
  //      {
  //        // Cleanup component handle references
  //        for (U32 i = 0; i < RS_MAX_COMPONENTS; ++i)
  //        {
  //          if (Component* component = it->second->GetComponent(i))
  //          {
  //            component->UnMountHandles();
  //          }
  //        }
  //        // Destroy actor and proxy
  //        delete actor;
  //        delete it->second;
  //        it = mActors.erase(it);
  //      }
  //      if (it != mActors.end())
  //      {
  //        ++it;
  //      }
  //    }
  //  }
  //}

  template<typename ... Cs>
  requires (std::is_base_of_v<Component, typename TypeProxy<Cs>::Type> && ...)
  void DispatchFor(std::function<void(typename TypeProxy<Cs>::Ptr ...)>&& predicate)
  {
    // Compute hash
    U64 bucketHash = ((U64)0 ^ ... ^ typeid(Cs).hash_code());
    // Execute predicate over actors in bucket
    for (auto const& proxy : mPermutationTable[bucketHash])
    {
      predicate
      (
        (proxy->GetActor()->GetComponent<typename TypeProxy<Cs>::Type>(typeid(Cs).hash_code()))
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

  GladGLContext* mGladContext = nullptr;
  ImGuiContext* mImGuiContext = nullptr;

  ModulesByName mModules = {};
  ResourcesByName mResources = {};
  ActorsByName mActors = {};

  HandlesByNameByType mHandles = {};

  PermutationTable mPermutationTable = {};
};