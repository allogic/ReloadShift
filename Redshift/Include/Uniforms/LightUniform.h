#pragma once

#include <Core.h>

struct alignas(16) UniformAmbientLight
{
  R32 Intensity;
  R32 Dummy0;
  R32 Dummy1;
  R32 Dummy2;
  R32V4 Color;
  U32 Enabled;
  U32 Dummy3;
  U32 Dummy4;
  U32 Dummy5;
};

struct alignas(16) UniformDirectionLight
{
  R32 Intensity;
  R32 Dummy0;
  R32 Dummy1;
  R32 Dummy2;
  R32V4 Color;
  R32V4 Direction;
  U32 Enabled;
  U32 Dummy3;
  U32 Dummy4;
  U32 Dummy5;
};

struct alignas(16) UniformPointLight
{
  R32 Intensity;
  R32 Dummy0;
  R32 Dummy1;
  R32 Dummy2;
  R32V4 Color;
  R32V4 Position;
  U32 Enabled;
  U32 Dummy3;
  U32 Dummy4;
  U32 Dummy5;
};

struct alignas(16) UniformSpotLight
{
  R32 Intensity;
  R32 CutOff;
  R32 Dummy0;
  R32 Dummy1;
  R32V4 Color;
  R32V4 Position;
  R32V4 Direction;
  U32 Enabled;
  U32 Dummy2;
  U32 Dummy3;
  U32 Dummy4;
};