#pragma once

#include <Core.h>

#pragma pack(push, 1)
struct Projection
{
  R32M4 Projection;
  R32M4 View;
  R32M4 Model;
};
#pragma pack(pop)