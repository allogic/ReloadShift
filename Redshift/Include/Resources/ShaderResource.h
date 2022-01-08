#pragma once

#include <Core.h>
#include <Resource.h>

#include <Handles/GenericShader.h>

#include <World.h>

template<EShaderType::Type Type>
class ShaderResource : public Resource
{
public:

  ShaderResource(
    World* world,
    std::string const& name,
    std::filesystem::path const& filePath)
    : Resource(world, name, filePath)
  {

  }

public:

  virtual inline std::string GetType() const override { return "Shader"; }

public:

  virtual bool LoadFile() override
  {
    if (!mBytes)
    {
      FILE* file = nullptr;
      errno_t error = fopen_s(&file, mFilePath.string().c_str(), "rb");
      if (error == 0 && file)
      {
        fseek(file, 0, SEEK_END);
        U32 fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        mBytes = new U8[fileSize];
        mBytesSize = fileSize;
        fread(mBytes, 1, fileSize, file);
        fclose(file);
        return mBytes;
      }
    }
    return false;
  }
  virtual bool ProduceHandles() override
  {
    U32 SID;
    SID = glCreateShader(Type);
    glShaderBinary(1, &SID, GL_SHADER_BINARY_FORMAT_SPIR_V, mBytes, (I32)mBytesSize);
    glSpecializeShader(SID, "main", 0, nullptr, nullptr);
    I32 compileStatus = 0;
    glGetShaderiv(SID, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus)
    {
      HotRef<GenericShader<Type>>& hotRef = mWorld->GetHandle<GenericShader<Type>>(GetName());
      if (hotRef.Get())
      {
        // Compare old values and decide if it has been changed at all
      }
      else
      {
        GenericShader<Type>* shader = new GenericShader<Type>{ GetName(), SID };
        shader->AddReference(this);
        hotRef.Set(shader);
      }
      mDirty = false;
    }
    else
    {
      I32 infoLogLength = 0;
      glGetShaderiv(SID, GL_INFO_LOG_LENGTH, &infoLogLength);
      std::string infoLog;
      infoLog.resize(infoLogLength);
      glGetShaderInfoLog(SID, infoLogLength, &infoLogLength, infoLog.data());
      std::printf("%s\n", infoLog.c_str());
      glDeleteShader(SID);
    }
    return compileStatus;
  }
};