#pragma once

#include <Core.h>
#include <Handle.h>

#include <Handles/GenericShader.h>

template<EProgramType::Type Type>
class GenericProgram : public Handle
{
public:

  GenericProgram(std::string const& name)
    : Handle(typeid(GenericProgram).name(), name)
  {

  }

public:

  virtual void Create() override
  {
    mID = glCreateProgram();
  }
  virtual void Destroy() override
  {
    glDeleteProgram(mID);
  }

public:

  void Bind()
  {
    glUseProgram(mID);
  }
  void Execute(U32 x, U32 y, U32 z)
  {
    static_assert(Type == EProgramType::Compute);
    glDispatchCompute(x, y, z);
  }

  template<typename ... Shaders>
  bool Link(Shaders* ... shaders)
  {
    (glAttachShader(mID, shaders->GetID()), ...);
    glLinkProgram(mID);
    I32 linkStatus = 0;
    glGetProgramiv(mID, GL_LINK_STATUS, &linkStatus);
    if (linkStatus)
    {

    }
    else
    {
      I32 infoLogLength = 0;
      glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &infoLogLength);
      std::string infoLog;
      infoLog.resize(infoLogLength);
      glGetProgramInfoLog(mID, infoLogLength, &infoLogLength, infoLog.data());
      std::printf("%s\n", infoLog.c_str());
      mDirty = true;
    }
    (glDetachShader(mID, shaders->GetID()), ...);
    return linkStatus;
  }

  void UnBind()
  {
    glUseProgram(0);
  }

  U32 mID;
};

using RenderProgram = GenericProgram<EProgramType::Render>;
using ComputeProgram = GenericProgram<EProgramType::Compute>;