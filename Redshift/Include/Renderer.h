#pragma once

#include <Core.h>

#include <Handles/GenericFrameBuffer.h>
#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>

class World;

class Renderer
{
public:

  Renderer(
    World* world,
    std::string const& name)
    : mWorld{ world }
    , mName{ name }
  {

  }
  virtual ~Renderer() = default;

public:

  inline std::string const& GetName() const { return mName; }
  inline R32V2 const& GetViewportSize() const { return mViewportSize; }
  inline U32 GetCurrentBufferID() const { return mCurrentBufferID; }
  inline std::map<std::string, U32> const& GetBuffers() const { return mBuffers; }

public:

  void SetCurrentBufferID(U32 value) { mCurrentBufferID = value; }
  void SetViewportSize(R32V2 const& value)
  {
    if (mViewportSize != value)
    {
      mViewportSize = value;
      ResizeViewport();
    }
  }

public:

  virtual void Render() = 0;
  virtual void ResizeViewport() = 0;

protected:

  World* mWorld;
  std::string mName;

  R32V2 mViewportSize = R32V2{};
  U32 mCurrentBufferID = 0;

  std::map<std::string, U32> mBuffers = {};
};