#pragma once

#include <Core.h>

template<typename T>
struct TypeProxy
{
  using Type = T;

  using Ref = T&;
  using CRef = T const&;

  using Ptr = T*;
  using CPtr = T const*;
};