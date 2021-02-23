#include "Engine/Renderer/VertexBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer( RenderContext* ctx, eRenderMemoryHint hint )
	:RenderBuffer(ctx,VERTEX_BUFFER_BIT,hint)
{
}

//////////////////////////////////////////////////////////////////////////
void VertexBuffer::Update(unsigned int vertexCount, void const* vertexData, unsigned int vertexStride, buffer_attribute_t const* layout)
{
	m_stride = (size_t)vertexStride;
	m_attributes = layout;
	m_count = (size_t)vertexCount;

	RenderBuffer::Update(vertexData, m_stride * m_count, m_stride);
}
