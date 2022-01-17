#pragma once

#include <Core.h>
#include <Actor.h>
#include <Handle.h>
#include <ModuleProxy.h>
#include <HotRef.h>
#include <Utility.h>
#include <Resource.h>
#include <Renderer.h>
#include <Delegates.h>
#include <Window.h>

#include <Components/Transform.h>
#include <Components/Rigidbody.h>

class Component;
class HotLoader;
class ActorProxy;

class World
{
public:

  ////////////////////////////////////////////////////////
  // Primitives
  ////////////////////////////////////////////////////////

  using ModuleMap = std::map<std::string, ModuleProxy*>;
  using ResourceMap = std::map<std::string, Resource*>;
  using ActorMap = std::map<Actor*, ActorProxy*>;
  using RendererMap = std::map<std::string, Renderer*>;

  using HotRefMapUnordered = std::unordered_map<std::string, HotRef<Handle>>;
  using HandleMapUnordered = std::unordered_map<U64, HotRefMapUnordered>;
  using PermutationMapUnordered = std::unordered_map<U64, std::set<ActorProxy*>>;

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

  inline GLFWwindow* GetGlfwContext() const { return mGlfwContext; }
  inline GladGLContext* GetGladContext() const { return mGladContext; }
  inline ImGuiContext* GetImGuiContext() const { return mImGuiContext; }

  inline ModuleMap& GetModules() { return mModules; }
  inline ResourceMap& GetResources() { return mResources; }
  inline ActorMap& GetActors() { return mActors; }
  inline RendererMap& GetRenderer() { return mRenderer; }

  inline HandleMapUnordered& GetHandles() { return mHandles; }
  inline PermutationMapUnordered& GetPermutations() { return mPermutations; }

  inline Delegates& GetDelegates() { return mDelegates; }

public:

  ////////////////////////////////////////////////////////
  // Resource interface
  ////////////////////////////////////////////////////////

  template<typename R, typename ... Args>
  requires std::is_base_of_v<Resource, R>
  static R* CreateResource(World& world, std::string const& resourceName, Args &&... args)
  {
    Resource*& resource = world.mResources[resourceName + ':' + typeid(R).name()];
    // Probe if resource already exists
    if (!resource)
    {
      resource = new R{ resourceName, std::forward<Args>(args) ... };
      // Enable auto load file and produce handles
      if (resource->LoadFile())
      {
        resource->LinkHandle();
      }
      resource->Cleanup();
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

private:

  ////////////////////////////////////////////////////////
  // Component utilities
  ////////////////////////////////////////////////////////

  static void RegisterProxyForAllPermutations(World& world, Actor* actor) noexcept
  {
    auto inOrderComponentHashes = actor->GetInOrderComponentHashes();
    // Register proxy for all single components
    for (auto const& componentHash : inOrderComponentHashes)
    {
      world.mPermutations[componentHash].emplace(actor->GetProxy());
    }
    // Complete transaction by computing all type permutations
    // and insert the proxy in their respective buckets
    do
    {
      // Sequencially compute hashes from current permutation
      for (U32 i = 0; i < inOrderComponentHashes.size(); ++i)
      {
        U64 permutationHash = 0;
        for (U32 j = i; j < inOrderComponentHashes.size(); ++j)
        {
          permutationHash ^= inOrderComponentHashes[j];
          world.mPermutations[permutationHash].emplace(actor->GetProxy());
        }
      }
    } while (std::next_permutation(inOrderComponentHashes.begin(), inOrderComponentHashes.end()));
  }
  static void DeRegisterProxyForAllPermutations(World& world, Actor* actor) noexcept
  {
    auto inOrderComponentHashes = actor->GetInOrderComponentHashes();
    // DeRegister proxy from all single components
    for (auto const& componentHash : inOrderComponentHashes)
    {
      world.mPermutations[componentHash].erase(actor->GetProxy());
    }
    // Complete transaction by computing all type permutations
    // and remove the proxy from their respective buckets
    do
    {
      // Sequencially compute hashes from current permutation
      for (U32 i = 0; i < inOrderComponentHashes.size(); ++i)
      {
        U64 permutationHash = 0;
        for (U32 j = i; j < inOrderComponentHashes.size(); ++j)
        {
          permutationHash ^= inOrderComponentHashes[j];
          world.mPermutations[permutationHash].erase(actor->GetProxy());
        }
      }
    } while (std::next_permutation(inOrderComponentHashes.begin(), inOrderComponentHashes.end()));
  }

public:

  ////////////////////////////////////////////////////////
  // Actor/Component interface
  ////////////////////////////////////////////////////////

  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  static C* AttachComponent(World& world, Actor* actor, Args &&... args)
  {
    // Check if component already exists
    C* component = actor->GetComponent<C>();
    if (!component)
    {
      // Update actor
      component = actor->CreateComponent<C>(new C{ world, std::forward<Args>(args) ... });
      // Register proxy in permutation table
      RegisterProxyForAllPermutations(world, actor);
    }
    return component;
  }

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  static A* CreateActor(World& world, std::string const& actorName, Args &&... args)
  {
    // Create actor proxy
    ActorProxy* proxy = new ActorProxy{};
    // Create actor after proxy in order to initialize components inside the constructor
    A* actor = new A{ world, proxy, actorName, std::forward<Args>(args) ... };
    // Link actor and proxy
    proxy->SetActor(actor);
    // Register input delegates
    actor->SetupInput(world.mDelegates);
    // Register actor
    world.mActors.emplace(actor, proxy);
    return actor;
  }

  template<typename A>
  requires std::is_base_of_v<Actor, A>
  static void DestroyActor(World& world, A* actor)
  {
    auto const actorIt = world.mActors.find(actor);
    if (actorIt != world.mActors.end())
    {
      // DeRegister actor
      world.mActors.erase(actor);
      // Destroy components
      actor->GetProxy()->DestroyAllComponents();
      // DeRegister proxy in permutation table
      DeRegisterProxyForAllPermutations(world, actor);
      // DeRegister event delegates
      world.mDelegates.UnBindAll(actor);
      // Delete actor and proxy
      delete actor->GetProxy();
      delete actor;
    }
  }

  static void DestroyAllActors(World& world)
  {
    for (auto it = world.mActors.begin(); it != world.mActors.end();)
    {
      Actor* actor = it->first;
      // DeRegister actor
      it = world.mActors.erase(it);
      // Destroy components
      actor->GetProxy()->DestroyAllComponents();
      // DeRegister proxy in permutation table
      DeRegisterProxyForAllPermutations(world, actor);
      // DeRegister event delegates
      world.mDelegates.UnBindAll(actor);
      // Delete actor and proxy
      delete actor->GetProxy();
      delete actor;
    }
  }

  template<typename ... Cs>
  requires (std::is_base_of_v<Component, typename TypeProxy<Cs>::Type> && ...)
  static void DispatchFor(World& world, std::function<void(typename TypeProxy<Cs>::Ptr ...)>&& predicate)
  {
    // Compute hash
    U64 bucketHash = ((U64)0 ^ ... ^ typeid(Cs).hash_code());
    // Execute predicate over actors in bucket
    for (auto const& proxy : world.mPermutations[bucketHash])
    {
      predicate
      (
        (proxy->GetActor()->GetComponent<typename TypeProxy<Cs>::Type>())
        ...
      );
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Module interface
  ////////////////////////////////////////////////////////

  static void UpdateModules(World& world, R32 deltaTime)
  {
    for (auto const& [name, proxy] : world.mModules)
    {
      proxy->GetModInstance()->Tick(deltaTime);
    }
  }
  static bool CreateModule(World& world, std::filesystem::path const& filePath);
  static bool DestroyModule(World& world, std::string const& moduleName);

public:

  ////////////////////////////////////////////////////////
  // Renderer interface
  ////////////////////////////////////////////////////////

  static void UpdateRenderer(World& world)
  {
    for (auto const& [name, renderer] : world.mRenderer)
    {
      renderer->Render();
    }
  }

  template<typename R, typename ... Args>
  requires std::is_base_of_v<Renderer, R>
  static R* CreateRenderer(World& world, std::string const& rendererName, Args &&... args)
  {
    Renderer*& renderer = world.mRenderer[rendererName];
    // Probe if renderer already exists
    if (!renderer)
    {
      renderer = new R{ world, rendererName, std::forward<Args>(args) ... };
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

public:

  ////////////////////////////////////////////////////////
  // Window interface
  ////////////////////////////////////////////////////////

  static void HideCursor(World& world)
  {
    world.mWindow.HideCursor(); // only works within main thread
  }
  static void ShowCursor(World& world)
  {
    world.mWindow.ShowCursor(); // only works within main thread
  }

public:

  ////////////////////////////////////////////////////////
  // Physics interface
  ////////////////////////////////////////////////////////

  static void UpdatePhysics(World& world, R32 timeStep)
  {
    // Update physics world
    world.mDynamicsWorld.stepSimulation(timeStep);
    // Update actor transforms
    // Important: Move this inside render loop and update every fixed 1.0f / 60
    DispatchFor<
      Transform,
      Rigidbody>(world, [](Transform* transform, Rigidbody* rigidbody)
        {
          btMotionState const* motionState = rigidbody->GetBody()->getMotionState();
          if (motionState)
          {
            btTransform worldTransform;
            motionState->getWorldTransform(worldTransform);
            btVector3 const& worldPosition = worldTransform.getOrigin();
            btQuaternion const& worldRotation = worldTransform.getRotation();
            transform->SetWorldPosition(R32V3{ worldPosition.x(), -worldPosition.y(), worldPosition.z() });
            transform->SetWorldRotation(R32V3{ worldRotation.y(), worldRotation.x(), worldRotation.z() });
          }
        });
  }
  static void CreatePhysicsBody(World& world, Rigidbody* rigidbody)
  {
    world.mDynamicsWorld.addRigidBody(rigidbody->GetBody());
  }
  static void DestroyPhysicsBody(World& world, Rigidbody* rigidbody)
  {
    world.mDynamicsWorld.removeRigidBody(rigidbody->GetBody());
  }

private:

  ////////////////////////////////////////////////////////
  // Saved context
  ////////////////////////////////////////////////////////

  GLFWwindow* mGlfwContext;
  GladGLContext* mGladContext;
  ImGuiContext* mImGuiContext;

private:

  ////////////////////////////////////////////////////////
  // Miscellaneous state
  ////////////////////////////////////////////////////////

  Delegates mDelegates = Delegates{ mGlfwContext };
  Window mWindow = Window{ mGlfwContext };

  ModuleMap mModules = ModuleMap{};
  ResourceMap mResources = ResourceMap{};
  ActorMap mActors = ActorMap{};
  RendererMap mRenderer = RendererMap{};

  HandleMapUnordered mHandles = HandleMapUnordered{};
  PermutationMapUnordered mPermutations = PermutationMapUnordered{};

private:

  ////////////////////////////////////////////////////////
  // Physics state
  ////////////////////////////////////////////////////////

  btDefaultCollisionConfiguration mCollisionConfiguration = btDefaultCollisionConfiguration{};
  btCollisionDispatcher mDispatcher = btCollisionDispatcher{ &mCollisionConfiguration };
  btDbvtBroadphase mOverlappingPairCache = btDbvtBroadphase{};
  btSequentialImpulseConstraintSolver mSolver = btSequentialImpulseConstraintSolver{};
  btDiscreteDynamicsWorld mDynamicsWorld = btDiscreteDynamicsWorld{ &mDispatcher, &mOverlappingPairCache, &mSolver, &mCollisionConfiguration };
};