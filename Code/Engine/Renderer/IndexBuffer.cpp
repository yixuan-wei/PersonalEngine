#include "Engine/Renderer/IndexBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
IndexBuffer::IndexBuffer(RenderContext* ctx, eRenderMemoryHint hint)
    :RenderBuffer(ctx,INDEX_BUFFER_BIT,hint)
{
}

//////////////////////////////////////////////////////////////////////////
void IndexBuffer::Update(std::vector<unsigned int> const& indices)
{
    Update((unsigned int)indices.size(), &indices[0]);
}

//////////////////////////////////////////////////////////////////////////
void IndexBuffer::Update(unsigned int iCount, unsigned int const* indices)
{
    m_count = iCount;
    RenderBuffer::Update(indices, iCount * sizeof(unsigned int), sizeof(unsigned int));
}
