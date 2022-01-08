#pragma once

#include <Core.h>

class Actor;

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
  inline U32 GetReferenceCount() const { return (U32)mActorReferences.size(); }

public:

  inline void AddReference(Actor* actor)
  {
    mActorReferences.emplace(actor);
  }
  inline void RemoveReference(Actor* actor)
  {
    mActorReferences.erase(mActorReferences.find(actor));
  }

public:

  inline void SetDirty(bool value) { mDirty = value; }

protected:

  std::string mType;
  std::string mName;

  bool mDirty = true;

  U32 mReferenceCount = 0;

  std::set<Actor*> mActorReferences = {};
};