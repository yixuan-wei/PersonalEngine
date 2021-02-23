#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/buffer_attribute_t.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
DXGI_FORMAT ToDXGIFormat(eBufferFormatType type)
{
    switch (type)
    {
    case BUFFER_FORMAT_VEC2:           return DXGI_FORMAT_R32G32_FLOAT;
    case BUFFER_FORMAT_VEC3:           return DXGI_FORMAT_R32G32B32_FLOAT;
    case BUFFER_FORMAT_VEC4:           return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case BUFFER_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
    default: ERROR_AND_DIE("buffer_attribute_t has no such format");
    }
}

//////////////////////////////////////////////////////////////////////////
D3D11_COMPARISON_FUNC ToDXCompareFunc(eCompareFunc func)
{
    switch (func)
    {
    case COMPARE_FUNC_NEVER:    return D3D11_COMPARISON_NEVER;
    case COMPARE_FUNC_ALWAYS:   return D3D11_COMPARISON_ALWAYS;
    case COMPARE_FUNC_LEQUAL:   return D3D11_COMPARISON_LESS_EQUAL;
    case COMPARE_FUNC_GEQUAL:   return D3D11_COMPARISON_GREATER_EQUAL;
    case COMPARE_FUNC_GREATER:  return D3D11_COMPARISON_GREATER;
    case COMPARE_FUNC_EQUAL:    return D3D11_COMPARISON_EQUAL;
    default: ERROR_AND_DIE("CompareFunc has no such function");
    }
}

//////////////////////////////////////////////////////////////////////////
D3D11_USAGE ToDXMemoryUsage(eRenderMemoryHint hint)
{
    switch (hint)
    {
    case MEMORY_HINT_DYNAMIC: return D3D11_USAGE_DYNAMIC;
    case MEMORY_HINT_GPU:     return D3D11_USAGE_DEFAULT;
    case MEMORY_HINT_STAGING: return D3D11_USAGE_STAGING;
    default: ERROR_AND_DIE("Unknown hint");
    }
}

//////////////////////////////////////////////////////////////////////////
D3D11_CULL_MODE ToDXCullMode(eCullMode mode)
{
    switch (mode)
    {
    case CULL_NONE:     return D3D11_CULL_NONE;
    case CULL_FRONT:    return D3D11_CULL_FRONT;
    case CULL_BACK:     return D3D11_CULL_BACK;
    default: ERROR_AND_DIE("Unknown cull mode");
    }
}

//////////////////////////////////////////////////////////////////////////
D3D11_FILL_MODE ToDXFillMode(eFillMode mode)
{
    switch (mode)
    {
    case FILL_SOLID:        return D3D11_FILL_SOLID;
    case FILL_WIREFRAME:    return D3D11_FILL_WIREFRAME;
    default: ERROR_AND_DIE("Unknown fill mode");
    }
}

//////////////////////////////////////////////////////////////////////////
UINT ToDXUsage(RenderBufferUsage usage)
{
    UINT ret = 0;

    if (usage & VERTEX_BUFFER_BIT)
    {
        ret |= D3D11_BIND_VERTEX_BUFFER;
    }

    if (usage & INDEX_BUFFER_BIT)
    {
        ret |= D3D11_BIND_INDEX_BUFFER;
    }

    if (usage & UNIFORM_BUFFER_BIT)
    {
        ret |= D3D11_BIND_CONSTANT_BUFFER;
    }

    return ret;
}