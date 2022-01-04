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
    : Handle(typeid(GenericShader).name(), name)
    , mID{ ID }
  {

  }

public:

  inline U32 GetID() const { return mID; }

public:

  virtual void Create() override
  {

  }
  virtual void Destroy() override
  {
    glDeleteShader(mID);
  }

private:

  U32 mID;
};

using VertexShader = GenericShader<EShaderType::Vertex>;
using FragmentShader = GenericShader<EShaderType::Fragment>;
using ComputeShader = GenericShader<EShaderType::Compute>;