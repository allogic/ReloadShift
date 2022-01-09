#define REDSHIFT_IMPLEMENTATION
#include <Redshift.h>

#include <Filesystem/FileBrowser.h>
#include <Filesystem/HotLoader.h>

#include <UI.h>

I32 main()
{
  // Set global error callback
  glfwSetErrorCallback([](I32 error, char const* msg) { std::printf(msg); });
  // Initialize GLFW
  if (!glfwInit())
  {
    std::printf("Failed initializing GLFW\n");
    return 0;
  }
  // Create main window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  U32 editorWidth = 1920;
  U32 editorHeight = 1080;
  GLFWwindow* window = glfwCreateWindow((I32)editorWidth, (I32)editorHeight, "Redshift", nullptr, nullptr);
  if (window)
  {
    // Set GC current
    glfwMakeContextCurrent(window);
    // Load GL
    if (gladLoadGL(glfwGetProcAddress))
    {
      // Disable V-Sync
      glfwSwapInterval(0);
      // Initialize ImGui
      IMGUI_CHECKVERSION();
      ImGuiContext* imGuiContext = ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO(); (void)io;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
      ImGui::StyleColorsDark();
      ImGuiStyle& style = ImGui::GetStyle();
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        style.WindowRounding = 0.0f;
        style.FrameBorderSize = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
      }
      bool imguiGLFWInitialized = ImGui_ImplGlfw_InitForOpenGL(window, true);
      bool imguiOGLinitialized = ImGui_ImplOpenGL3_Init("#version 460 core");
      if (imGuiContext && imguiGLFWInitialized && imguiOGLinitialized)
      {
        // Create global world object
        World& world = World::Instance();
        // Create file browser and hot-reloader
        HotLoader hotLoader = { window, &world, "C:\\Users\\Michael\\Downloads\\Redshift\\Streaming\\" };
        FileBrowser fileBrowser = { "" };
        // Setup timer stuff
        R32 prevTime = 0.0f;
        R32 renderRate = 1.0f / 60.0f;
        R32 prevRenderTime = 0.0f;
        R32 hotLoadRate = 1.0f / 1.0f;
        R32 prevHotLoadTime = 0.0f;
        try
        {
          // Enter main loop
          while (!glfwWindowShouldClose(window))
          {
            OPTICK_FRAME("MainLoop");
            // Compute delta time
            R32 time = (R32)glfwGetTime();
            R32 deltaTime = time - prevTime;
            // Dispatch modules render
            if ((time - prevRenderTime) >= renderRate)
            {
              // Render
              for (auto const& [name, proxy] : world.GetModules())
              {
                proxy->GetModInstance()->Render();
              }
              // Safe prev render time
              prevRenderTime = time;
            }
            // Update hot-loader
            if ((time - prevHotLoadTime) >= hotLoadRate)
            {
              // Update hot-loader
              hotLoader.Update();
              // Safe prev hot-load time
              prevHotLoadTime = time;
            }
            // Begin rendering imgui
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, (I32)editorWidth, (I32)editorHeight);
            // Create new imgui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // Draw editor imgui
            UI::Draw(&world);
            // Tick modules and draw inline imgui
            for (auto const& [name, proxy] : world.GetModules())
            {
              proxy->GetModInstance()->Tick(deltaTime);
            }
            // Render all imgui windows
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(window);
            // Swap buffers as fast as possible
            glfwSwapBuffers(window);
            // Poll occuring events
            glfwPollEvents();
            // Safe prev time
            prevTime = time;
          }
        }
        catch (std::exception const& exception)
        {
          std::printf("%s\n", exception.what());
        }
        // Shutdown ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        // clear world
        // clear hotloads
      }
    }
    else
    {
      std::printf("Failed loading GL\n");
    }
    // Destroy GC
    glfwDestroyWindow(window);
  }
  else
  {
    std::printf("Failed creating window\n");
  }
  // Shutdown GLFW
  glfwTerminate();
  return 0;
}