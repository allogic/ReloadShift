#pragma once

#include <Core.h>

class UI
{
public:

  UI(GLFWwindow* window);

public:

  bool Initialize();
  void Begin();
  void Render();
  void End();
  void Shutdown();

private:

  GLFWwindow* mGLFWWindow = nullptr;
  ImGuiContext* mImGuiContext = nullptr;
};