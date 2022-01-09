#pragma once

#include <Core.h>
#include <Resource.h>
#include <Vertex.h>

class MeshResource : public Resource
{
public:

  MeshResource(
    World* world,
    std::string const& name,
    std::filesystem::path const& filePath);

public:

  virtual inline std::string GetType() const override { return "Mesh"; }

public:

  virtual bool LoadFile() override;
  virtual bool ProduceHandles() override;

  bool ParseScene();

private:

  std::vector<Vertex> mVertices = {};
  std::vector<U32> mElements = {};
};