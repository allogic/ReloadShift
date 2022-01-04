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

  virtual bool LoadFile() override;
  virtual bool ProduceHandles() override;

private:

  static ofbx::IElement const* GetElementByName(ofbx::IElement const* parent, std::string const& elementName);
  static ofbx::IElementProperty const* GetPropertyByName(ofbx::IElement const* parent, std::string const& propertyName);

  template<typename T>
  static void GetArray(std::vector<T>& buffer, ofbx::IElementProperty const* property)
  {
    I32 size = property->getCount();
    buffer.resize(size);
    property->getValues(&buffer[0], (I32)sizeof(buffer[0]) * size);
  }

  bool ParseScene();

private:

  std::vector<Vertex> mVertices = {};
  std::vector<U32> mIndices = {};
};