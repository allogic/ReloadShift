#pragma once

#include <Core.h>

struct alignas(16) UniformCamera
{
  R32M4 Projection;
  R32M4 View;
};