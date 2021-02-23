#pragma once

#include "Engine/Renderer/RenderBuffer.hpp"

struct buffer_attribute_t;

//////////////////////////////////////////////////////////////////////////
class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer( RenderContext* ctx, eRenderMemoryHint hint );

    void Update(unsigned int vertexCount, void const* vertexData, unsigned int vertexStride, buffer_attribute_t const* layout);

public:
    size_t m_stride; // how far from one vertex to the next
    buffer_attribute_t const* m_attributes; // array describing the vertex
    size_t m_count = 0;
};