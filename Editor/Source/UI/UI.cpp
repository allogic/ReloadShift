#include <UI/UI.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

UI::UI(GLFWwindow* window)
  : mGLFWWindow{ window }
{

}

bool UI::Initialize()
{
  if (mGLFWWindow)
  {
    IMGUI_CHECKVERSION();
    mImGuiContext = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    bool imguiGLFWInitialized = ImGui_ImplGlfw_InitForOpenGL(mGLFWWindow, true);
    bool imguiOGLinitialized = ImGui_ImplOpenGL3_Init("#version 460 core");

    return mImGuiContext && imguiGLFWInitialized && imguiOGLinitialized;
  }
  return false;
}
void UI::Begin()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}
void UI::Render()
{
  ImGui::Render();
  I32 width = 0;
  I32 height = 0;
  glfwGetFramebufferSize(mGLFWWindow, &width, &height);
  glViewport(0, 0, width, height);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void UI::End()
{
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
}
void UI::Shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}