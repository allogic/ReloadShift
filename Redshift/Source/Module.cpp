#include <Module.h>

#include <Globals/World.h>

Module::Module(World& world)
  : mWorld{ world }
{
  gladSetGLContext(mWorld.GetGladContext());
}

void Module::Tick(R32 deltaTime)
{
  gladSetGLContext(mWorld.GetGladContext());
  ImGui::SetCurrentContext(mWorld.GetImGuiContext());
}