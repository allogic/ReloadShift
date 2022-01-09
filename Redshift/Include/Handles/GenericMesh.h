#pragma once

#include <Core.h>
#include <Handle.h>
#include <Vertex.h>

#include <Handles/GenericBuffer.h>

template<typename Vertex, typename Element>
class GenericMesh : public Handle
{
public:

  GenericMesh(std::string const& name)
    : Handle(name)
  {
    glGenVertexArrays(1, &mID);
  }
  virtual ~GenericMesh()
  {
    glDeleteVertexArrays(1, &mID);
  }

public:

  virtual inline std::string GetType() const override { return "Mesh"; }

public:

  void Bind() const
  {
    glBindVertexArray(mID);
  }
  bool Link(VertexBuffer<Vertex>* vertexBuffer, ElementBuffer<Element>* elementBuffer)
  {
    glBindVertexArray(mID);
    vertexBuffer->Bind();
    U64 stride = 0;
    for (U32 i = 0; i < (sizeof(Vertex::Strides) / sizeof(Vertex::Strides[0])); ++i)
    {
      glEnableVertexAttribArray(i);
      glVertexAttribPointer(i, Vertex::Strides[i], GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)stride);
      stride += Vertex::Strides[i];
    }
    elementBuffer->Bind();
    mNumElements = elementBuffer->GetSize();
    glBindVertexArray(0);
    vertexBuffer->UnBind();
    elementBuffer->UnBind();
    return true;
  }
  void Draw() const
  {
    glDrawElements(GL_TRIANGLES, mNumElements, GL_UNSIGNED_INT, nullptr);
  }
  void UnBind() const
  {
    glBindVertexArray(0);
  }

  U32 mID = 0;
  U32 mNumElements = 0;
};

using DefaultMesh = GenericMesh<Vertex, U32>;