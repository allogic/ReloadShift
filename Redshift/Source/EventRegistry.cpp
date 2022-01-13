#include <EventRegistry.h>

EventRegistry::EventRegistry(GLFWwindow* context)
  : mGlfwContext{ context }
{

}

void EventRegistry::Poll()
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