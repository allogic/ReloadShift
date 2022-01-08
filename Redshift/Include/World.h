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
  inline std::vector<ActorProxy*> GetActors() { return mActors; }

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
  H* MountHandle(std::string const& handleName, Args &&... args)
  {
    // Compute keys
    std::string handleKey = std::string{ typeid(H).name() } + ':' + handleName;
    // Check if existing handles exist
    auto const handleIt = mHandles.equal_range(handleKey);
    if (handleIt.first == handleIt.second)
    {
      // Insert new handle
      auto const& emplaceIt = mHandles.emplace(handleKey, new H{ handleName, std::forward<Args>(args) ... });
      emplaceIt->second->Create();
      return (H*)emplaceIt->second;
    }
    else
    {
      // Return first non dirty handle
      return GetFirstNonDirtyHandleByName<H>(handleName);
    }
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
      //actor->SetHash(actorHash);
      component = actor->CreateComponent<C>(new C{ this, std::forward<Args>(args) ... });
      // Register proxy for all single types
      auto permutations = actor->GetInOrderComponentHashes();
      for (auto const& typHash : permutations)
      {
        mTypeBuckets[typHash].emplace(actor->GetProxy());
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
            mTypeBuckets[permutationHash].emplace(actor->GetProxy());
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
    // Create actor after proxy in order to initialize components inside the constructor
    A* actor = new A{ this, proxy, std::forward<Args>(args) ... };
    actor->SetName(actorName);
    proxy->SetActor(actor);
    // Add proxy to actor list
    mActors.emplace_back(proxy);
    // Add proxy to empty component bucket
    mTypeBuckets[actor->GetCurrentHash()].emplace(proxy);
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
  void DispatchFor(std::function<void(typename TypeProxy<Cs>::Ptr ...)>&& predicate)
  {
    // Compute hash
    U64 bucketHash = ((U64)0 ^ ... ^ typeid(Cs).hash_code());
    // Execute predicate over actors in bucket
    for (auto const& proxy : mTypeBuckets[bucketHash])
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

  ////////////////////////////////////////////////////////
  // Context state
  ////////////////////////////////////////////////////////

  GladGLContext* mGladContext = nullptr;
  ImGuiContext* mImGuiContext = nullptr;

private:

  ////////////////////////////////////////////////////////
  // Modules
  ////////////////////////////////////////////////////////

  std::map<std::string, ModuleProxy*> mModules = {};

private:

  ////////////////////////////////////////////////////////
  // Resources
  ////////////////////////////////////////////////////////

  std::map<std::string, Resource*> mResources = {};

private:

  ////////////////////////////////////////////////////////
  // Handles
  ////////////////////////////////////////////////////////

  std::multimap<std::string, Handle*> mHandles = {};

private:

  ////////////////////////////////////////////////////////
  // Actor/Components
  ////////////////////////////////////////////////////////

  std::map<U64, std::set<ActorProxy*>> mTypeBuckets = {};
  std::vector<ActorProxy*> mActors = {};
  U32 mUniqueComponentCount = 0;
  std::map<U64, U32> mIdentities = {};
};