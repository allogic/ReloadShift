#pragma once

#include <Core.h>

class Window
{
public:

  static GLFWwindow* Create(U32 width, U32 height, std::string const& title);
};