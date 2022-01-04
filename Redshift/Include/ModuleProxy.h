#pragma once

#include <Core.h>
#include <Module.h>

class ModuleProxy
{
public:

  ModuleProxy(
    HINSTANCE winInstance,
    Module* modInstance,
    Module::CreateProc createProc,
    Module::DestroyProc destroyProc)
    : mWinInstance{ winInstance }
    , mModInstance{ modInstance }
    , mCreateProc{ createProc }
    , mDestroyProc{ destroyProc }
  {

  }
  virtual ~ModuleProxy() = default;

public:

  inline HINSTANCE GetWinInstance() const { return mWinInstance; }
  inline Module* GetModInstance() const { return mModInstance; }
  inline Module::CreateProc GetCreateProx() const { return mCreateProc; }
  inline Module::DestroyProc GetDestroyProx() const { return mDestroyProc; }

private:

  HINSTANCE mWinInstance;

  Module* mModInstance;

  Module::CreateProc mCreateProc;
  Module::DestroyProc mDestroyProc;
};