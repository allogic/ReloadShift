#pragma once

#include <Core.h>

#pragma pack(push, 1)
struct UniformAmbientLight
{
  R32 Intensity;
  R32V3 Color;
  U32 Enabled;
};
struct UniformDirectionLight
{
  R32 Intensity;
  R32V3 Color;
  R32V3 Direction;
  U32 Enabled;
};
struct UniformPointLight
{
  R32 Intensity;
  R32V3 Color;
  R32V3 Position;
  U32 Enabled;
};
struct UniformSpotLight
{
  R32 Intensity;
  R32 CutOff;
  R32V3 Color;
  R32V3 Position;
  R32V3 Direction;
  U32 Enabled;
};
#pragma pack(pop)