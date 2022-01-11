#pragma once

#include <Core.h>
#include <Actor.h>
#include <Handle.h>
#include <ModuleProxy.h>
#include <HotRef.h>
#include <Utility.h>
#include <Resource.h>
#include <Renderer.h>
#include <EventRegistry.h>

class Component;
class HotLoader;
class ActorProxy;

class World
{
public:

  ////////////////////////////////////////////////////////
  // Primitives
  ////////////////////////////////////////////////////////

  using ModulesByName = std::map<std::string, ModuleProxy*>;
  using ResourcesByName = std::map<std::string, Resource*>;
  using ActorsByName = std::unordered_multimap<std::string, ActorProxy*>;
  using RendererByName = std::map<std::string, Renderer*>;

  using HandlesByName = std::unordered_map<std::string, HotRef<Handle>>;
  using HandlesByNameByType = std::unordered_map<U64, HandlesByName>;

  using PermutationTable = std::unordered_map<U64, std::set<ActorProxy*>>;

public:

  ////////////////////////////////////////////////////////
  // Singleton
  ////////////////////////////////////////////////////////

  static World& Instance();

public:

  ////////////////////////////////////////////////////////
  // Constructor
  ////////////////////////////////////////////////////////

  World();

public:

  ////////////////////////////////////////////////////////
  // Getter
  ////////////////////////////////////////////////////////

  inline GLFWwindow* GetGlfwWindow() const { return mGlfwWindow; }
  inline GladGLContext* GetGladContext() const { return mGladContext; }
  inline ImGuiContext* GetImGuiContext() const { return mImGuiContext; }

  inline ModulesByName& GetModules() { return mModules; }
  inline ResourcesByName& GetResources() { return mResources; }
  inline ActorsByName& GetActors() { return mActors; }
  inline RendererByName& GetRenderer() { return mRenderer; }

  inline HandlesByNameByType& GetHandles() { return mHandles; }

  inline PermutationTable& GetPermutationTable() { return mPermutationTable; }

  inline EventRegistry& GetEventRegistry() { return mEventRegistry; }

public:

  ////////////////////////////////////////////////////////
  // Resource interface
  ////////////////////////////////////////////////////////

  template<typename R, typename ... Args>
  requires std::is_base_of_v<Resource, R>
  static R* CreateResource(World& world, std::string const& resourceName, Args &&... args)
  {
    Resource*& resource = world.mResources[resourceName + typeid(R).name()];
    // Probe if resource already exists
    if (!resource)
    {
      resource = new R{ resourceName, std::forward<Args>(args) ... };
      // Enable auto load file and produce handles
      if (resource->LoadFile())
      {
        resource->ProduceHandles();
      }
      return (R*)resource;
    }
    return (R*)resource;
  }

  template<typename R>
  requires std::is_base_of_v<Resource, R>
  static bool DestroyResource(World& world, std::string const& resourceName)
  {
    Resource*& resource = world.mResources[resourceName + typeid(R).name()];
    // Check if resource exists, if so, destroy it
    if (resource)
    {
      delete resource; resource = nullptr;
      return true;
    }
    return false;
  }

public:

  ////////////////////////////////////////////////////////
  // Handle utilities
  ////////////////////////////////////////////////////////

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  static void ConsumeDirtyHandleNames(World& world, std::set<std::string>& handleNames) noexcept
  {
    for (auto const& [name, hotRef] : world.mHandles[typeid(H).hash_code()])
    {
      if (hotRef.Get() && hotRef.Get()->GetDirty())
      {
        handleNames.emplace(name);
      }
    }
  }

  template<typename H>
  requires std::is_base_of_v<Handle, H>
  static void ConsumeNonDirtyHandleNames(World& world, std::set<std::string>& handleNames) noexcept
  {
    for (auto const& [name, hotRef] : world.mHandles[typeid(H).hash_code()])
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
  static HotRef<H>& LinkHandle(World& world, std::string const& handleName, Args &&... args)
  {
    HotRef<H>& hotRef = (HotRef<H>&)world.mHandles[typeid(H).hash_code()][handleName];
    // Probe if reference already exists
    if (!hotRef.Get())
    {
      // Create new handle
      hotRef.Set(new H{ handleName, std::forward<Args>(args) ... });
    }
    return hotRef;
  }

  template<typename H, typename ... Args>
  requires std::is_base_of_v<Handle, H>
  static HotRef<H>& GetHandle(World& world, std::string const& handleName)
  {
    return (HotRef<H>&)world.mHandles[typeid(H).hash_code()][handleName];
  }

  template<typename H, typename ... Args>
  requires std::is_base_of_v<Handle, H>
  static void TrySetDirty(World& world, std::string const& handleName)
  {
    auto const outerIt = world.mHandles.find(typeid(H).hash_code());
    if (outerIt != world.mHandles.end())
    {
      auto const innerIt = outerIt->second.find(handleName);
      if (innerIt != outerIt->second.end())
      {
        if (innerIt->second.Get())
        {
          innerIt->second.Get()->SetDirty(true);
        }
      }
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Actor/Component interface
  ////////////////////////////////////////////////////////

  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  static C* AttachComponent(World& world, Actor* actor, Args && ... args)
  {
    // Check if component already exists
    C* component = actor->GetComponent<C>(typeid(C).hash_code());
    if (!component)
    {
      // Update actor
      component = actor->CreateComponent<C>(new C{ world, std::forward<Args>(args) ... });
      // Register proxy for all single types
      auto permutations = actor->GetInOrderComponentHashes();
      for (auto const& typHash : permutations)
      {
        world.mPermutationTable[typHash].emplace(actor->GetProxy());
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
            world.mPermutationTable[permutationHash].emplace(actor->GetProxy());
          }
        }
      } while (std::next_permutation(permutations.begin(), permutations.end()));
    }
    return component;
  }

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  static A* CreateActor(World& world, std::string const& actorName, Args && ... args)
  {
    // Create actor proxy
    ActorProxy* proxy = new ActorProxy{};
    world.mActors.emplace(actorName, proxy);
    // Create actor after proxy in order to initialize components inside the constructor
    A* actor = new A{ world, proxy, actorName, std::forward<Args>(args) ... };
    // Register input mapping
    actor->SetupInput(world.mEventRegistry);
    // Link actor and proxy
    proxy->SetActor(actor);
    return actor;
  }

  template<typename ... Cs>
  requires (std::is_base_of_v<Component, typename TypeProxy<Cs>::Type> && ...)
  static void DispatchFor(World& world, std::function<void(typename TypeProxy<Cs>::Ptr ...)>&& predicate)
  {
    // Compute hash
    U64 bucketHash = ((U64)0 ^ ... ^ typeid(Cs).hash_code());
    // Execute predicate over actors in bucket
    for (auto const& proxy : world.mPermutationTable[bucketHash])
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

  static bool CreateModule(World& world, std::filesystem::path const& filePath);
  static bool DestroyModule(World& world, std::string const& moduleName);

public:

  ////////////////////////////////////////////////////////
  // Renderer interface
  ////////////////////////////////////////////////////////

  template<typename R, typename ... Args>
  requires std::is_base_of_v<Renderer, R>
  static R* CreateRenderer(World& world, std::string const& rendererName, Args &&... args)
  {
    Renderer*& renderer = world.mRenderer[rendererName];
    // Probe if renderer already exists
    if (!renderer)
    {
      renderer = new R{ rendererName, std::forward<Args>(args) ... };
      return (R*)renderer;
    }
    return (R*)renderer;
  }

  template<typename R>
  requires std::is_base_of_v<Renderer, R>
  static bool DestroyRenderer(World& world, std::string const& rendererName)
  {
    Renderer*& renderer = world.mResources[rendererName];
    // Check if resource exists, if so, destroy it
    if (renderer)
    {
      delete renderer; renderer = nullptr;
      return true;
    }
    return false;
  }

private:

  GLFWwindow* mGlfwWindow;
  GladGLContext* mGladContext;
  ImGuiContext* mImGuiContext;

  EventRegistry mEventRegistry = EventRegistry{ mGlfwWindow };

  ModulesByName mModules = ModulesByName{};
  ResourcesByName mResources = ResourcesByName{};
  ActorsByName mActors = ActorsByName{};
  RendererByName mRenderer = RendererByName{};

  HandlesByNameByType mHandles = HandlesByNameByType{};

  PermutationTable mPermutationTable = PermutationTable{};
};