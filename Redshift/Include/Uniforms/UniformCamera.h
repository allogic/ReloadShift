#pragma once

#include <Core.h>

#pragma pack(push, 1)
struct CameraUniform
{
  R32M4 Projection;
  R32M4 View;
};
#pragma pack(pop)