#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
RenderBuffer::RenderBuffer( RenderContext* owner, RenderBufferUsage usage, eRenderMemoryHint memHint )
	:m_owner(owner)
	,m_usage(usage)
	,m_memHint(memHint)
	,m_handle(nullptr)
	,m_bufferByteSize(0U)
	,m_elementByteSize(0U)
{
}

//////////////////////////////////////////////////////////////////////////
RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE( m_handle );
}

//////////////////////////////////////////////////////////////////////////
bool RenderBuffer::Update( void const* data, size_t dataByteSize, size_t elementSize )
{
	if( !IsCompatible(dataByteSize,elementSize) )
	{
		CleanUp();//destroy buffer
		if (dataByteSize <= 0) {
			return false;
		}

		Create(dataByteSize, elementSize);
	}

	ID3D11DeviceContext* ctx = m_owner->m_context;
	if( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		//only available for DYNAMIC buffer, but don't have to reallocate is going smaller
		D3D11_MAPPED_SUBRESOURCE mapped;

		HRESULT result = ctx->Map( m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
		if( SUCCEEDED( result ) )
		{
			memcpy( mapped.pData, data, dataByteSize );
			ctx->Unmap( m_handle, 0 );
		}
		else
		{
			return false;
		}
	}
	else
	{
	    //only available to GPU buffer with SAME size/element size
		ctx->UpdateSubresource( m_handle, 0, nullptr, data, 0, 0 );
	}
	
	m_elementByteSize = elementSize;
	m_bufferByteSize = dataByteSize;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool RenderBuffer::IsCompatible( size_t dataByteSize, size_t elementByteSize )
{
	if( m_handle == nullptr )
	{
		return false;
	}

	if( m_elementByteSize != elementByteSize ) //element size don't match
	{
		return false;
	}

	if( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		return dataByteSize <= m_bufferByteSize;
	}
	else
	{
		return m_bufferByteSize == dataByteSize;
	}
}


//////////////////////////////////////////////////////////////////////////
bool RenderBuffer::Create( size_t dataByteSize, size_t elementByteSize )
{
	ID3D11Device* device = m_owner->m_device;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (UINT)dataByteSize;
	desc.Usage = ToDXMemoryUsage( m_memHint );
	desc.BindFlags = ToDXUsage( m_usage );
	desc.MiscFlags = 0;//special cases
	desc.StructureByteStride = (UINT)elementByteSize;
	desc.CPUAccessFlags = 0U;
	if( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if( m_memHint == MEMORY_HINT_STAGING )
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	device->CreateBuffer( &desc, nullptr, &m_handle );

	return (m_handle != nullptr);
}

//////////////////////////////////////////////////////////////////////////
void RenderBuffer::CleanUp()
{
	DX_SAFE_RELEASE( m_handle );
	m_bufferByteSize = 0;
	m_elementByteSize = 0;
}
