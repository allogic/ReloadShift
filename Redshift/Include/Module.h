#pragma once

#include <Core.h>

class World;

class Module
{
public:

  using CreateProc = Module* (*)(World&);
  using DestroyProc = void (*)(Module*);

public:

  Module(World& world);
  virtual ~Module() = default;

public:

  virtual void Tick(R32 deltaTime);

protected:

  World& mWorld;
};

extern "C"
{
  __declspec(dllexport) Module* CreateModule(World& world);
  __declspec(dllexport) void DestroyModule(Module* ptr);
}

#define DECLARE_MODULE_IMPL(TYPE)                                \
Module* CreateModule(World& world) { return new TYPE{ world }; } \
void DestroyModule(Module* ptr) { delete ptr; }