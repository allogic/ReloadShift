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

    bool operator < (ActionKey const& other) const { return Key < other.Key; }
  };

  using AxisDelegates = std::unordered_map<std::string, AxisInfo>;
  using ActionDelegates = std::map<ActionKey, std::multiset<ActionInfo>>;

  struct Event
  {
    EInputState::Type Current;
    EInputState::Type Previous;
  };

public:

  ////////////////////////////////////////////////////////
  // Constructor
  ////////////////////////////////////////////////////////

  EventRegistry(GLFWwindow* context);

public:

  ////////////////////////////////////////////////////////
  // Getter
  ////////////////////////////////////////////////////////

  inline AxisDelegates const& GetAxisDelegates() const { return mAxisDelegates; }
  inline ActionDelegates const& GetActionDelegates() const { return mActionDelegates; }

public:

  void Poll();

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
  void BindAction(U32 key, EInputState::Type state, A const* actor, void(A::*actionDelegate)())
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