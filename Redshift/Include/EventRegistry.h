#pragma once

#include <Core.h>
#include <Types.h>

class Actor;

class EventRegistry
{
public:

  ////////////////////////////////////////////////////////
  // Primitives
  ////////////////////////////////////////////////////////

  using AxisDelegate = void (Actor::*)(float);
  using ActionDelegate = void (Actor::*)();

  struct AxisInfo
  {
    Actor* Instance;
    AxisDelegate Delegate;
  };
  struct ActionInfo
  {
    Actor* Instance;
    ActionDelegate Delegate;

    bool operator < (ActionInfo const& other) const { return Instance < other.Instance; }
  };

  struct ActionKey
  {
    U32 Key;
    EInputState::Type State;

    bool operator == (ActionKey const& other) const { return Key == other.Key && State == other.State; }
  };

  struct ActionHasher
  {
    U64 operator () (ActionKey const& actionKey) const { return actionKey.Key ^ actionKey.State; }
  };

  using AxisDelegates = std::unordered_map<std::string, AxisInfo>;
  using ActionDelegates = std::unordered_map<ActionKey, std::multiset<ActionInfo>, ActionHasher>;

  struct Event
  {
    EInputState::Type Current;
    EInputState::Type Previous;
  };

public:

  ////////////////////////////////////////////////////////
  // Constructor
  ////////////////////////////////////////////////////////

  EventRegistry(GLFWwindow* context)
    : mGlfwContext{ context }
  {

  }

public:

  ////////////////////////////////////////////////////////
  // Getter
  ////////////////////////////////////////////////////////

  inline AxisDelegates const& GetAxisDelegates() const { return mAxisDelegates; }
  inline ActionDelegates const& GetActionDelegates() const { return mActionDelegates; }

public:

  ////////////////////////////////////////////////////////
  // Event forwarding
  // Note: Do not put this function inside source file
  //       otherwise it cannot execute delegates properly
  ////////////////////////////////////////////////////////

  void Poll()
  {
    // Poll events
    glfwPollEvents();
    // Update mouse button states
    for (U32 i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i)
    {
      Event& event = mMouseKeyStates[i];
      U32 action = glfwGetMouseButton(mGlfwContext, (I32)i);

      event.Previous = event.Current;

      if (action == GLFW_PRESS)
      {
        if (event.Current != EInputState::Pressed && event.Previous != EInputState::Held)
        {
          event.Current = EInputState::Pressed;
        }
        else
        {
          event.Current = EInputState::Held;
        }
      }
      if (action == GLFW_RELEASE)
      {
        if (event.Current != EInputState::Released && event.Previous == EInputState::Held)
        {
          event.Current = EInputState::Released;
        }
        else
        {
          event.Current = EInputState::None;
        }
      }
    }
    // Update keyboard button states
    for (U32 i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; ++i)
    {
      Event& event = mKeyboardKeyStates[i];
      I32 action = glfwGetKey(mGlfwContext, (I32)i);

      if (action == GLFW_PRESS)
      {
        if (event.Current != EInputState::Pressed && event.Previous != EInputState::Held)
        {
          event.Current = EInputState::Pressed;
        }
        else
        {
          event.Current = EInputState::Held;
        }
      }
      if (action == GLFW_RELEASE)
      {
        if (event.Current != EInputState::Released && event.Previous == EInputState::Held)
        {
          event.Current = EInputState::Released;
        }
        else
        {
          event.Current = EInputState::None;
        }
      }
    }
    // Forward keyboard & mouse key states
    for (auto const& [actionKey, actionInfos] : mActionDelegates)
    {
      if (mMouseKeyStates[actionKey.Key].Current == actionKey.State)
      {
        // Execute delegate for all registered instances
        for (auto const& actionInfo : actionInfos)
        {
          ((*actionInfo.Instance).*(actionInfo.Delegate))();
        }
      }
      if (mKeyboardKeyStates[actionKey.Key].Current == actionKey.State)
      {
        // Execute delegate for all registered instances
        for (auto const& actionInfo : actionInfos)
        {
          ((*actionInfo.Instance).*(actionInfo.Delegate))();
        }
      }
    }
  }

public:

  ////////////////////////////////////////////////////////
  // Input interface
  ////////////////////////////////////////////////////////

  template<typename A>
  requires std::is_base_of_v<Actor, A>
  void BindAxis(std::string const& axisName, A* actor, void(A::* axisDelegate)(float))
  {
    mAxisDelegates[axisName].Instance = (Actor*)actor;
    mAxisDelegates[axisName].Delegate = (AxisDelegate)axisDelegate;
  }

  template<typename A>
  requires std::is_base_of_v<Actor, A>
  void BindAction(U32 key, EInputState::Type state, A* actor, void(A::* actionDelegate)())
  {
    mActionDelegates[ActionKey{ key, state }].emplace(ActionInfo{ (Actor*)actor, (ActionDelegate)actionDelegate });
  }

private:

  GLFWwindow* mGlfwContext;

  AxisDelegates mAxisDelegates = AxisDelegates{};
  ActionDelegates mActionDelegates = ActionDelegates{};

  Event mMouseKeyStates[GLFW_MOUSE_BUTTON_LAST] = {};
  Event mKeyboardKeyStates[GLFW_KEY_LAST] = {};
};