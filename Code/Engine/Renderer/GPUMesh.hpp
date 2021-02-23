#pragma once

class VertexBuffer;
class IndexBuffer;
struct buffer_attribute_t;
class RenderContext;

class GPUMesh
{
public:
    GPUMesh(RenderContext* owner);
    ~GPUMesh();

    void UpdateVertices(unsigned int vCount, void const* vertexData, unsigned int vStride, buffer_attribute_t const* layout);
    void UpdateIndices(unsigned int iCount, unsigned int const* indices);

    int GetIndexCount() const;
    int GetVertexCount() const;

    template <typename VERTEX_TYPE>
    void UpdateVertices(unsigned int vCount, VERTEX_TYPE const* vertices)
    {
        UpdateVertices(vCount, vertices, sizeof(VERTEX_TYPE), VERTEX_TYPE::LAYOUT);
    }

public:
    VertexBuffer* m_vertices = nullptr;
    IndexBuffer* m_indices = nullptr;
};