#pragma once

#include <Core.h>
#include <Resource.h>

#include <Handles/GenericShader.h>

template<EShaderType::Type Type>
class ShaderResource : public Resource
{
public:

  ShaderResource(
    std::string const& name,
    std::filesystem::path const& filePath)
    : Resource(name, filePath)
  {

  }

public:

  virtual inline std::string GetType() const override { return "Shader"; }

public:

  virtual bool LoadFile() override
  {
    FILE* file = nullptr;
    errno_t error = fopen_s(&file, mFilePath.string().c_str(), "rb");
    if (error == 0 && file)
    {
      fseek(file, 0, SEEK_END);
      mFileSize = ftell(file);
      fseek(file, 0, SEEK_SET);
      mShaderBytes = new U8[mFileSize];
      fread(mShaderBytes, 1, mFileSize, file);
      fclose(file);
      // Create shader
      mID = glCreateShader(Type);
      glShaderBinary(1, &mID, GL_SHADER_BINARY_FORMAT_SPIR_V, mShaderBytes, mFileSize);
      glSpecializeShader(mID, "main", 0, nullptr, nullptr);
      I32 compileStatus = 0;
      glGetShaderiv(mID, GL_COMPILE_STATUS, &compileStatus);
      if (!compileStatus)
      {
        I32 infoLogLength = 0;
        glGetShaderiv(mID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::string infoLog;
        infoLog.resize(infoLogLength);
        glGetShaderInfoLog(mID, infoLogLength, &infoLogLength, infoLog.data());
        std::printf("%s\n", infoLog.c_str());
        glDeleteShader(mID);
      }
      return compileStatus;
    }
    return false;
  }
  virtual void LinkHandle() override
  {
    HotRef<GenericShader<Type>>& hotRef = World::GetHandle<GenericShader<Type>>(mWorld, GetName());
    if (hotRef.Get())
    {
      // Compare old values and decide if it has been changed at all
    }
    else
    {
      GenericShader<Type>* shader = new GenericShader<Type>{ GetName(), mID };
      shader->AddReference(this);
      hotRef.Set(shader);
    }
  }
  virtual void Cleanup() override
  {
    delete mShaderBytes; mShaderBytes = nullptr;
    mDirty = false;
  }

private:

  U32 mID = 0;

  U8* mShaderBytes = nullptr;
};