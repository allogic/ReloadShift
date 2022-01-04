#pragma once

#include <Core.h>
#include <Handle.h>

template<U32 Type, typename Value>
class GenericBuffer : public Handle
{
public:

  GenericBuffer(
    std::string const& name,
    U32 size)
    : Handle(typeid(GenericBuffer).name(), name)
    , mSize{ size }
  {

  }

public:

  virtual void Create() override
  {
    glGenBuffers(1, &mID);
    glBindBuffer(Type, mID);
    glBufferStorage(Type, mSize * sizeof(Value), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    glBindBuffer(Type, 0);
  }
  virtual void Destroy() override
  {
    glDeleteBuffers(1, &mID);
  }

public:

  void Bind()
  {
    glBindBuffer(Type, mID);
  }
  void Mount(U32 index)
  {
    static_assert(Type == GL_SHADER_STORAGE_BUFFER || Type == GL_UNIFORM_BUFFER);
    glBindBufferBase(Type, index, mID);
  }
  void Clear()
  {
    glBindBuffer(Type, mID);
    glBufferSubData(Type, 0, mSize * sizeof(Value), nullptr);
    glBindBuffer(Type, 0);
  }
  void Set(Value* buffer)
  {
    glBindBuffer(Type, mID);
    glBufferSubData(Type, 0, mSize * sizeof(Value), buffer);
    glBindBuffer(Type, 0);
  }
  void Set(U32 offset, Value* buffer, U32 size)
  {
    glBindBuffer(Type, mID);
    glBufferSubData(Type, offset * sizeof(Value), size * sizeof(Value), buffer);
    glBindBuffer(Type, 0);
  }
  void Get(Value* buffer)
  {
    glBindBuffer(Type, mID);
    glGetBufferSubData(Type, 0, mSize * sizeof(Value), buffer);
    glBindBuffer(Type, 0);
  }
  void Get(U32 offset, Value* buffer, U32 size)
  {
    glBindBuffer(Type, mID);
    glGetBufferSubData(Type, offset * sizeof(Value), size * sizeof(Value), buffer);
    glBindBuffer(Type, 0);
  }
  void UnBind()
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