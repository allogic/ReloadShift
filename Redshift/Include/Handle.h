#pragma once

#include <Core.h>

class Handle
{
public:

  Handle(std::string const& name)
    : mName{ name }
  {

  }
  virtual ~Handle() = default;

public:

  virtual inline std::string GetType() const { return ""; }

public:

  inline std::string const& GetName() const { return mName; }
  inline bool GetDirty() const { return mDirty; }
  inline U32 GetReferenceCount() const { return (U32)mReferences.size(); }

public:

  inline void AddReference(void* actor)
  {
    mReferences.emplace(actor);
  }
  inline void RemoveReference(void* actor)
  {
    mReferences.erase(mReferences.find(actor));
  }

public:

  inline void SetDirty(bool value) { mDirty = value; }

protected:

  std::string mName;

  bool mDirty = false;

  U32 mReferenceCount = 0;

  std::set<void*> mReferences = {};
};