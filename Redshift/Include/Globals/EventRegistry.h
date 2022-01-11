#pragma once

#include <Core.h>

class Actor;

class EventRegistry
{
public:

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
  };

  using AxisDelegates = std::unordered_map<std::string, AxisInfo>;
  using ActionDelegates = std::unordered_map<std::string, ActionInfo>;

public:

  static inline EventRegistry& Instance() { static EventRegistry registry; return registry; }

public:

  EventRegistry() = default;

public:

  inline AxisDelegates const& GetAxisDelegates() const { return mAxisDelegates; }
  inline ActionDelegates const& GetActionDelegates() const { return mActionDelegates; }

public:

  template<typename A>
  requires std::is_base_of_v<Actor, A>
  void BindAxis(std::string const& axisName, A const* actor, void(A::*axisDelegate)(float))
  {
    mAxisDelegates[axisName].Instance = (Actor*)actor;
    mAxisDelegates[axisName].Delegate = (AxisDelegate)axisDelegate;
  }

  template<typename A>
  requires std::is_base_of_v<Actor, A>
  void BindAction(std::string const& actionName, EInputType::Type inputType, A const* actor, void(A::*actionDelegate)())
  {
    mActionDelegates[actionName].Instance = (Actor*)actor;
    mActionDelegates[actionName].Delegate = (ActionDelegate)actionDelegate;
  }

private:

  AxisDelegates mAxisDelegates = AxisDelegates{};
  ActionDelegates mActionDelegates = ActionDelegates{};
};