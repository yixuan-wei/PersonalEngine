#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
GPUMesh::GPUMesh(RenderContext* owner)
{
    m_indices = new IndexBuffer(owner, MEMORY_HINT_DYNAMIC);
    m_vertices = new VertexBuffer(owner, MEMORY_HINT_DYNAMIC);
}

//////////////////////////////////////////////////////////////////////////
GPUMesh::~GPUMesh()
{
    delete m_indices;
    delete m_vertices;
}

//////////////////////////////////////////////////////////////////////////
void GPUMesh::UpdateVertices(unsigned int vCount, void const* vertexData, unsigned int vStride, buffer_attribute_t const* layout)
{
    m_vertices->Update(vCount, vertexData, vStride, layout);
}

//////////////////////////////////////////////////////////////////////////
void GPUMesh::UpdateIndices(unsigned int iCount, unsigned int const* indices)
{
    m_indices->Update(iCount, indices);
}

//////////////////////////////////////////////////////////////////////////
int GPUMesh::GetIndexCount() const
{
    return m_indices->GetIndexCount();
}

//////////////////////////////////////////////////////////////////////////
int GPUMesh::GetVertexCount() const
{
    return (int)m_vertices->m_count;
}
