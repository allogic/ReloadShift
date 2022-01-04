#pragma once

#include <Core.h>

class Handle
{
public:

  Handle(
    std::string const& type,
    std::string const& name)
    : mType{ type }
    , mName{ name }
  {

  }
  virtual ~Handle() = default;

public:

  inline std::string const& GetType() const { return mType; }
  inline std::string const& GetName() const { return mName; }
  inline bool GetDirty() const { return mDirty; }
  inline U32 GetReferenceCount() const { return mReferenceCount; }

public:

  inline void IncReferenceCount() { mReferenceCount++; }
  inline void DecReferenceCount() { mReferenceCount--; }

public:

  inline void SetDirty(bool value) { mDirty = value; }
  inline void SetReferenceCount(U32 value) { mReferenceCount = value; }

public:

  virtual void Create() = 0;
  virtual void Destroy() = 0;

protected:

  std::string mType;
  std::string mName;

  bool mDirty = false;

  U32 mReferenceCount = 0;
};