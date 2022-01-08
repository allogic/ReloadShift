#pragma once

#include <Core.h>
#include <Handle.h>

template<EShaderType::Type Type>
class GenericShader : public Handle
{
public:

  GenericShader(
    std::string const& name,
    U32 ID)
    : Handle(name)
    , mID{ ID }
  {

  }
  virtual ~GenericShader()
  {
    glDeleteShader(mID);
  }

public:

  virtual inline std::string GetType() const override { return "Shader"; }

public:

  inline U32 GetID() const { return mID; }

private:

  U32 mID;
};

using VertexShader = GenericShader<EShaderType::Vertex>;
using FragmentShader = GenericShader<EShaderType::Fragment>;
using ComputeShader = GenericShader<EShaderType::Compute>;