#pragma once

#include <Core.h>

#pragma pack(push, 1)
class Vertex
{
public:

  constexpr static U32 Strides[5] = { 3, 3, 2, 4, 3 };

public:

  R32V3 Position;
  R32V3 Normal;
  R32V2 UV;
  R32V4 Color;
  R32V3 Tangent;
};
#pragma pack(pop)