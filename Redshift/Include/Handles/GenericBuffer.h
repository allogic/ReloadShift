#pragma once

#include <Core.h>
#include <Handle.h>

template<U32 Type, typename Value>
class GenericBuffer : public Handle
{
public:

  GenericBuffer(std::string const& name, U32 size)
    : Handle(name)
    , mSize{ size }
  {
    glGenBuffers(1, &mID);
    glBindBuffer(Type, mID);
    glBufferStorage(Type, mSize * sizeof(Value), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    glBindBuffer(Type, 0);
  }
  virtual ~GenericBuffer()
  {
    glDeleteBuffers(1, &mID);
  }

public:

  virtual inline std::string GetType() const override { return "Buffer"; }

public:

  inline U32 GetSize() const { return mSize; }

public:

  void Bind() const
  {
    glBindBuffer(Type, mID);
  }
  void Mount(U32 index) const
  {
    static_assert(Type == GL_SHADER_STORAGE_BUFFER || Type == GL_UNIFORM_BUFFER);
    glBindBufferBase(Type, index, mID);
  }
  void Clear() const
  {
    glBufferSubData(Type, 0, mSize * sizeof(Value), nullptr);
  }
  void Set(Value* buffer) const
  {
    glBufferSubData(Type, 0, mSize * sizeof(Value), buffer);
  }
  void Set(U32 offset, Value* buffer, U32 size) const
  {
    glBufferSubData(Type, offset * sizeof(Value), size * sizeof(Value), buffer);
  }
  void Get(Value* buffer) const
  {
    glGetBufferSubData(Type, 0, mSize * sizeof(Value), buffer);
  }
  void Get(U32 offset, Value* buffer, U32 size) const
  {
    glGetBufferSubData(Type, offset * sizeof(Value), size * sizeof(Value), buffer);
  }
  void UnBind() const
  {
    glBindBuffer(Type, mID);
  }

private:

  U32 mSize;

  U32 mID = 0;
};

template<typename Value>
using StorageBuffer = GenericBuffer<GL_SHADER_STORAGE_BUFFER, Value>;

template<typename Value>
using UniformBuffer = GenericBuffer<GL_UNIFORM_BUFFER, Value>;

template<typename Value>
using VertexBuffer = GenericBuffer<GL_ARRAY_BUFFER, Value>;

template<typename Value>
using ElementBuffer = GenericBuffer<GL_ELEMENT_ARRAY_BUFFER, Value>;