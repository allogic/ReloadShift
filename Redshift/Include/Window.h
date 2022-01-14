#pragma once

#include <Core.h>

class Window
{
public:

  Window(GLFWwindow* context)
    : mGlfwContext{ context }
  {

  }

public:

  void HideCursor()
  {
    glfwSetInputMode(mGlfwContext, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  void ShowCursor()
  {
    glfwSetInputMode(mGlfwContext, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

private:

  GLFWwindow* mGlfwContext;
};