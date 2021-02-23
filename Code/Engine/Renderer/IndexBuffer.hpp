#pragma once

#include "Engine/Renderer/RenderBuffer.hpp"
#include <vector>

class IndexBuffer : public RenderBuffer
{
public:
    IndexBuffer(RenderContext* ctx, eRenderMemoryHint hint);

    void Update(unsigned int iCount, unsigned int const* indices);
    void Update(std::vector<unsigned int> const& indices);

    int GetIndexCount() const { return (int)m_count; }

private:
    unsigned int m_count = 0;
};