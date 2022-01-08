#pragma once

#include <Core.h>
#include <Handle.h>

template<typename H>
requires std::is_base_of_v<Handle, H>
class HotRef
{
public:

  HotRef() = default;
  virtual ~HotRef() = default;

public:

  inline H* Get() const { return mHandle; }

public:

  inline void Set(H* handle) { mHandle = handle; }

private:

  H* mHandle = nullptr;
};