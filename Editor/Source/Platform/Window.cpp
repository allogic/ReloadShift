#include <Platform/Window.h>

GLFWwindow* Window::Create(U32 width, U32 height, std::string const& title)
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  return glfwCreateWindow((I32)width, (I32)height, title.c_str(), nullptr, nullptr);
}