#include <Resources/MeshResource.h>

#include <Handles/GenericBuffer.h>

#include <World.h>

MeshResource::MeshResource(
  World* world,
  std::string const& name,
  std::filesystem::path const& filePath)
  : Resource(world, name, filePath)
{

}

bool MeshResource::LoadFile()
{
  if (!mBytes)
  {
    FILE* file = nullptr;
    errno_t error = fopen_s(&file, mFilePath.string().c_str(), "rb");
    if (error == 0 && file)
    {
      fseek(file, 0, SEEK_END);
      U32 fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);
      mBytes = new U8[fileSize];
      mBytesSize = fileSize;
      fread(mBytes, 1, fileSize, file);
      fclose(file);
      return mBytes;
    }
  }
  return false;
}
bool MeshResource::ProduceHandles()
{
  if (ParseScene())
  {
    //mWorld->MarkHandlesAsDirtyByName<VertexBuffer<Vertex>>(GetName());
    //VertexBuffer<Vertex>* vertexBufferHandle = mWorld->MountHandle<VertexBuffer<Vertex>>(GetName(), (U32)mVertices.size());
    //vertexBufferHandle->Bind();
    //vertexBufferHandle->Set(mVertices.data());
    //vertexBufferHandle->UnBind();
    //mWorld->MarkHandlesAsDirtyByName<ElementBuffer<U32>>(GetName());
    //ElementBuffer<U32>* elementBufferHandle = mWorld->MountHandle<ElementBuffer<U32>>(GetName(), (U32)mElements.size());
    //elementBufferHandle->Bind();
    //elementBufferHandle->Set(mElements.data());
    //elementBufferHandle->UnBind();
    //mDirty = false;
  }
  return false;
}

ofbx::IElement const* MeshResource::GetElementByName(ofbx::IElement const* parent, std::string const& elementName)
{
  I8 label[512] = {};
  for (ofbx::IElement const* element = parent->getFirstChild(); element; element = element->getSibling())
  {
    element->getID().toString(label);
    if (std::strcmp(label, elementName.c_str()) == 0)
    {
      return element;
    }
  }
  return nullptr;
}
ofbx::IElementProperty const* MeshResource::GetPropertyByName(ofbx::IElement const* parent, std::string const& propertyName)
{
  I8 label[512] = {};
  for (ofbx::IElementProperty const* property = parent->getFirstProperty(); property; property = property->getNext())
  {
    std::printf("%c\n", (I8)property->getType());
    return property;
  }
  return nullptr;
}

bool MeshResource::ParseScene()
{
  if (mBytes)
  {
    ofbx::IScene* scene = ofbx::load(mBytes, mBytesSize, (U64)ofbx::LoadFlags::TRIANGULATE);
    if (scene && scene->getRootElement())
    {
      //scene->getMesh();

      ofbx::IElement const* root = scene->getRootElement();
      ofbx::IElement const* objects = GetElementByName(root, "Objects");
      ofbx::IElement const* geometry = GetElementByName(objects, "Geometry");
      ofbx::IElement const* vertices = GetElementByName(geometry, "Vertices");
      ofbx::IElement const* polygonVertexIndex = GetElementByName(geometry, "PolygonVertexIndex");

      std::vector<R64> positionsRaw;
      std::vector<I32> indicesRaw;

      GetArray(positionsRaw, vertices->getFirstProperty());
      GetArray(indicesRaw, polygonVertexIndex->getFirstProperty());

      mVertices.clear();
      mElements.clear();

      mVertices.resize(positionsRaw.size() / 3);
      for (U32 i = 0, j = 0; (i < mVertices.size()) && (j < positionsRaw.size()); ++i, j += 3)
      {
        mVertices[i].Position.x = (R32)positionsRaw[j + 0];
        mVertices[i].Position.y = (R32)positionsRaw[j + 1];
        mVertices[i].Position.z = (R32)positionsRaw[j + 2];
      }

      mElements.resize(indicesRaw.size());
      for (U32 i = 0; i < indicesRaw.size(); ++i)
      {
        mElements[i] = (U32)indicesRaw[i];
      }

      return true;
    }
  }
  return false;
}