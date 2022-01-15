#define REDSHIFT_IMPLEMENTATION
#include <Redshift.h>

#include <Filesystem/FileBrowser.h>
#include <Filesystem/HotLoader.h>

#include <UI/UI.h>

I32 main()
{
  // Set global error callback
  glfwSetErrorCallback([](I32 error, I8 const* msg) { std::printf(msg); });
  // Initialize GLFW
  if (!glfwInit())
  {
    std::printf("Failed initializing GLFW\n");
    return 0;
  }
  // Create global registry
  Registry& registry = Registry::Instance();
  // Set window parameters
  registry.Set<U32>("editor_width", 1920);
  registry.Set<U32>("editor_height", 1080);
  // Create main window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  GLFWwindow* window = glfwCreateWindow((I32)registry.Get<U32>("editor_width"), (I32)registry.Get<U32>("editor_height"), "Redshift", nullptr, nullptr);
  if (window)
  {
    // Make context current
    glfwMakeContextCurrent(window);
    // Load GL
    if (gladLoadGL(glfwGetProcAddress))
    {
      // Set global GL debug callback
      glEnable(GL_DEBUG_OUTPUT);
      glDebugMessageCallback([](U32 source, U32 type, U32 id, U32 severity, I32 length, I8 const* msg, void const* userParam)
        {
          switch (severity)
          {
            case GL_DEBUG_SEVERITY_NOTIFICATION: break;
            case GL_DEBUG_SEVERITY_LOW: std::printf("Severity:Low Type:0x%x Message:%s\n", type, msg); break;
            case GL_DEBUG_SEVERITY_MEDIUM: std::printf("Severity:Medium Type:0x%x Message:%s\n", type, msg); break;
            case GL_DEBUG_SEVERITY_HIGH: std::printf("Severity:High Type:0x%x Message:%s\n", type, msg); break;
          }
        }, 0);
      // Disable V-Sync
      glfwSwapInterval(0);
      // Initialize ImGui
      IMGUI_CHECKVERSION();
      ImGuiContext* imGuiContext = ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO();
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
        // Create global world
        World& world = World::Instance();
        // Create file browser and hot-reloader
        HotLoader hotLoader = HotLoader{ window, "C:\\Users\\Michael\\Downloads\\Redshift\\Streaming\\" };
        FileBrowser fileBrowser = FileBrowser{ "" };
        // Create default renderer
        World::CreateRenderer<DeferredRenderer>(world, "Deferred");
        // Setup timer stuff
        R32 prevTime = 0.0f;
        R32 renderRate = 1.0f / 60.0f;
        R32 prevRenderTime = 0.0f;
        R32 hotLoadRate = 1.0f / 1.0f;
        R32 prevHotLoadTime = 0.0f;
        R32 physicsRate = 1.0f / 60.0f;
        // FPS counter
        R32 prevTimeFPS = (R32)glfwGetTime();
        U32 FPSCount = 0;
        try
        {
          // Enter main loop
          while (!glfwWindowShouldClose(window))
          {
            OPTICK_FRAME("MainLoop");
            // Compute delta time
            R32 time = (R32)glfwGetTime();
            R32 deltaTime = time - prevTime;
            // FPS counter
            FPSCount++;
            if ((time - prevTimeFPS) > 1.0f)
            {
              registry.Set<U32>("fps_count", FPSCount);
              registry.Set<R32>("ms_per_frame", 1000.0f / FPSCount);
              FPSCount = 0;
              prevTimeFPS += 1.0f;
            }
            // Update renderer
            if ((time - prevRenderTime) >= renderRate)
            {
              World::UpdateRenderer(world);
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
            // Update physics
            World::UpdatePhysics(world, physicsRate);
            // Begin rendering imgui
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, (I32)registry.Get<U32>("editor_width"), (I32)registry.Get<U32>("editor_height"));
            // Create new imgui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // Draw editor imgui
            UI::Draw();
            // Update modules
            World::UpdateModules(world, deltaTime);
            // Render all imgui windows
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            // Make context current
            glfwMakeContextCurrent(window);
            // Swap buffers as fast as possible
            glfwSwapBuffers(window);
            // Poll events
            glfwPollEvents();
            // Poll occuring events
            world.GetDelegates().Update();
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
        // Destroy world here
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
  // Destroy registry here
  // Shutdown GLFW
  glfwTerminate();
  return 0;
}