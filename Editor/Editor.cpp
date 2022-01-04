#define REDSHIFT_IMPLEMENTATION
#include <Redshift.h>

#include <Filesystem/FileBrowser.h>
#include <Filesystem/HotLoader.h>

#include <Platform/Window.h>

#include <UI/UI.h>

I32 main()
{
  glfwSetErrorCallback([](I32 error, char const* msg) { std::printf(msg); });

  if (!glfwInit())
  {
    std::printf("Failed initializing GLFW\n");
    return 0;
  }

  GLFWwindow* window = Window::Create(1280, 720, "Redshift");

  if (window)
  {
    glfwMakeContextCurrent(window);

    if (gladLoadGL(glfwGetProcAddress))
    {
      glfwSwapInterval(0);

      UI ui = { window };

      R32 time = 0.0f;
      R32 prevTime = 0.0f;
      R32 deltaTime = 0.0f;
      R32 renderRate = 1.0f / 60.0f;
      R32 prevRenderTime = 0.0f;
      R32 hotLoadRate = 1.0f / 1.0f;
      R32 prevHotLoadTime = 0.0f;

      if (ui.Initialize())
      {
        World& world = World::Instance();
        HotLoader hotLoader = { window, &world, "C:\\Users\\Michael\\Downloads\\Redshift\\Streaming\\" };
        FileBrowser fileBrowser = { "" };

        try
        {
          while (!glfwWindowShouldClose(window))
          {
            time = (R32)glfwGetTime();
            deltaTime = time - prevTime;

            ui.Begin();

            for (auto const& [name, proxy] : world.GetModules())
            {
              proxy->GetModInstance()->Tick(deltaTime);
            }

            if ((time - prevRenderTime) >= renderRate)
            {
              for (auto const& [name, proxy] : world.GetModules())
              {
                proxy->GetModInstance()->Render(deltaTime);
              }

              prevRenderTime = time;
            }
            if ((time - prevHotLoadTime) >= hotLoadRate)
            {
              hotLoader.Update();

              for (auto const& [name, proxy] : world.GetModules())
              {
                proxy->GetModInstance()->Link();
              }

              prevHotLoadTime = time;
            }

            ui.Render();
            ui.End();

            glfwMakeContextCurrent(window);
            glfwPollEvents();
            glfwSwapBuffers(window);

            prevTime = time;
          }
        }
        catch (std::exception const& exception)
        {
          std::printf("%s\n", exception.what());
        }

        ui.Shutdown();

        // clear world
        // clear hotloads
      }
    }
    else
    {
      std::printf("Failed loading GL\n");
    }

    glfwDestroyWindow(window);
  }
  else
  {
    std::printf("Failed creating window\n");
  }

  glfwTerminate();

  return 0;
}