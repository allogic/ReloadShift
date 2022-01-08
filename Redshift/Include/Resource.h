#pragma once

#include <Core.h>
#include <Types.h>

class World;

class Resource
{
public:

  Resource(
    World* world,
    std::string const& name,
    std::filesystem::path const& filePath)
    : mWorld{ world }
    , mName{ name }
    , mFilePath{ filePath }
  {

  }
  virtual ~Resource()
  {
    if (mBytes)
    {
      delete mBytes;
    }
  }

public:

  virtual inline std::string GetType() const { return ""; }

public:

  inline std::string const& GetName() const { return mName; }
  inline std::filesystem::path GetFilePath() const { return mFilePath; }
  inline U8 const* GetBytes() const { return mBytes; }
  inline U32 GetBytesSize() const { return mBytesSize; }
  inline bool GetDirty() const { return mDirty; }

public:

  inline void SetDirty(bool value) { mDirty = value; }

public:

  virtual bool LoadFile() { return false; }
  virtual bool ProduceHandles() { return false; }

protected:

  World* mWorld;

  std::string mName;

  std::filesystem::path mFilePath;

  U8* mBytes = nullptr;
  U32 mBytesSize = 0;

  bool mDirty = true;
};