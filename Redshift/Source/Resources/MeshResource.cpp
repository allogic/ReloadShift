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
    {
      HotRef<VertexBuffer<Vertex>>& hotRef = mWorld->GetHandle<VertexBuffer<Vertex>>(GetName());
      if (hotRef.Get())
      {
        // Compare old values and decide if it has been changed at all
      }
      else
      {
        VertexBuffer<Vertex>* vertexBuffer = new VertexBuffer<Vertex>{ GetName(), (U32)mVertices.size() };
        vertexBuffer->AddReference(this);
        vertexBuffer->Bind();
        vertexBuffer->Set(mVertices.data());
        vertexBuffer->UnBind();
        hotRef.Set(vertexBuffer);
      }
    }

    {
      HotRef<ElementBuffer<U32>>& hotRef = mWorld->GetHandle<ElementBuffer<U32>>(GetName());
      if (hotRef.Get())
      {
        // Compare old values and decide if it has been changed at all
      }
      else
      {
        ElementBuffer<U32>* elementBuffer = new ElementBuffer<U32>{ GetName(), (U32)mElements.size() };
        elementBuffer->AddReference(this);
        elementBuffer->Bind();
        elementBuffer->Set(mElements.data());
        elementBuffer->UnBind();
        hotRef.Set(elementBuffer);
      }
    }
    
    mDirty = false;
    return true;
  }
  return false;
}

bool MeshResource::ParseScene()
{
  if (mBytes)
  {
    ofbx::IScene* scene = ofbx::load(mBytes, mBytesSize, (U64)ofbx::LoadFlags::TRIANGULATE);
    for (U32 i = 0; i < 1 /* scene->getMeshCount() */; ++i)
    {
      ofbx::Mesh const* mesh = scene->getMesh(i);
      ofbx::Geometry const* geometry = mesh->getGeometry();
      // Extract vertices
      U32 vertexCount = geometry->getVertexCount();
      mVertices.clear();
      mVertices.resize(vertexCount);
      ofbx::Vec3 const* vertices = geometry->getVertices();
      ofbx::Vec3 const* normals = geometry->getNormals();
      ofbx::Vec2 const* UVs = geometry->getUVs();
      ofbx::Vec4 const* colors = geometry->getColors();
      ofbx::Vec3 const* tangents = geometry->getTangents();
      for (U32 i = 0; i < vertexCount; ++i)
      {
        if (vertices)
        {
          mVertices[i].Position.x = (R32)vertices[i].x;
          mVertices[i].Position.y = (R32)vertices[i].y;
          mVertices[i].Position.z = (R32)vertices[i].z;
        }

        if (normals)
        {
          mVertices[i].Normal.x = (R32)normals[i].x;
          mVertices[i].Normal.y = (R32)normals[i].y;
          mVertices[i].Normal.z = (R32)normals[i].z;
        }

        if (UVs)
        {
          mVertices[i].UV.x = (R32)UVs[i].x;
          mVertices[i].UV.y = (R32)UVs[i].y;
        }

        if (colors)
        {
          mVertices[i].Color.x = (R32)colors[i].x;
          mVertices[i].Color.y = (R32)colors[i].y;
          mVertices[i].Color.z = (R32)colors[i].z;
          mVertices[i].Color.w = (R32)colors[i].w;
        }

        if (tangents)
        {
          mVertices[i].Tangent.x = (R32)tangents[i].x;
          mVertices[i].Tangent.y = (R32)tangents[i].y;
          mVertices[i].Tangent.z = (R32)tangents[i].z;
        }
      }
      // Extract elements
      U32 elementCount = geometry->getIndexCount();
      mElements.clear();
      mElements.resize(elementCount);
      I32 const* elements = geometry->getFaceIndices();
      for (U32 i = 0; i < (elementCount); i += 3)
      {
        mElements[i + 0] = (elements[i + 0] < 0) ? (-elements[i + 0] - 1) : elements[i + 0];
        mElements[i + 1] = (elements[i + 1] < 0) ? (-elements[i + 1] - 1) : elements[i + 1];
        mElements[i + 2] = (elements[i + 2] < 0) ? (-elements[i + 2] - 1) : elements[i + 2];
      }
    }
    return true;
  }
  return false;
}