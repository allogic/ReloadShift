#pragma once

#include <Core.h>
#include <Types.h>
#include <HotRef.h>

class World;

class Resource
{
public:

  Resource(std::string const& name, std::filesystem::path const& filePath);
  virtual ~Resource() = default;

public:

  virtual inline std::string GetType() const { return ""; }

public:

  inline std::string const& GetName() const { return mName; }
  inline std::filesystem::path const& GetFilePath() const { return mFilePath; }
  inline U32 GetFileSize() const { return mFileSize; }
  inline bool GetDirty() const { return mDirty; }

public:

  inline void SetDirty(bool value) { mDirty = value; }

public:

  virtual bool LoadFile() { return false; }
  virtual void LinkHandle() {}
  virtual void Cleanup() {}

protected:

  World& mWorld;

  std::string mName;

  std::filesystem::path mFilePath;

  U32 mFileSize = 0;

  bool mDirty = true;
};