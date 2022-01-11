#pragma once

#include <Core.h>

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

  using AxisDelegates = std::unordered_map<std::string, AxisInfo>;
  using ActionDelegates = std::map<U8, std::map<U32, std::multiset<ActionInfo>>>;

public:

  ////////////////////////////////////////////////////////
  // Constructor
  ////////////////////////////////////////////////////////

  EventRegistry(GLFWwindow* window)
  {
    // Set custom user pointer
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetKeyCallback(window, KeyCallback);
  }

public:

  ////////////////////////////////////////////////////////
  // Getter
  ////////////////////////////////////////////////////////

  inline AxisDelegates const& GetAxisDelegates() const { return mAxisDelegates; }
  inline ActionDelegates const& GetActionDelegates() const { return mActionDelegates; }

public:

  ////////////////////////////////////////////////////////
  // Input interface
  ////////////////////////////////////////////////////////

  template<typename A>
  requires std::is_base_of_v<Actor, A>
  void BindAxis(std::string const& axisName, A const* actor, void(A::*axisDelegate)(float))
  {
    mAxisDelegates[axisName].Instance = (Actor*)actor;
    mAxisDelegates[axisName].Delegate = (AxisDelegate)axisDelegate;
  }

  template<typename A>
  requires std::is_base_of_v<Actor, A>
  void BindAction(U8 actionKey, EInputType::Type inputType, A const* actor, void(A::*actionDelegate)())
  {
    mActionDelegates[actionKey][inputType].emplace(ActionInfo{ (Actor*)actor, (ActionDelegate)actionDelegate });
  }

private:

  ////////////////////////////////////////////////////////
  // Window callbacks
  ////////////////////////////////////////////////////////

  static void MouseCallback(GLFWwindow* window, R64 x, R64 y)
  {
    EventRegistry* registry = (EventRegistry*)glfwGetWindowUserPointer(window);
  }
  static void KeyCallback(GLFWwindow* window, I32 key, I32 scanCode, I32 action, I32 mods)
  {
    EventRegistry* registry = (EventRegistry*)glfwGetWindowUserPointer(window);
    std::printf("key:%d scanCode:%d action:%d mods:%d\n", key, scanCode, action, mods);
    // Search delegate
    for (auto const& actionInfo : registry->mActionDelegates[key][action])
    {
      // Execute delegate if possible
      if (actionInfo.Instance && actionInfo.Delegate)
      {
        ((*actionInfo.Instance).*(actionInfo.Delegate))();
      }
    }
  }

private:

  AxisDelegates mAxisDelegates = AxisDelegates{};
  ActionDelegates mActionDelegates = ActionDelegates{};
};