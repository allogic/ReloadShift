#pragma once

#include <Core.h>
#include <Resource.h>
#include <Vertex.h>

class MeshResource : public Resource
{
public:

  MeshResource(
    std::string const& name,
    std::filesystem::path const& filePath);

public:

  virtual inline std::string GetType() const override { return "Mesh"; }

public:

  virtual bool LoadFile() override;
  virtual void LinkHandle() override;
  virtual void Cleanup() override;

private:

  U32 mVertexCount = 0;
  U32 mElementCount = 0;

  Vertex* mVertices = nullptr;
  U32* mElements = nullptr;
};