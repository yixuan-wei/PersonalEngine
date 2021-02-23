#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

//////////////////////////////////////////////////////////////////////////
SwapChain::SwapChain( RenderContext* renderer, IDXGISwapChain* handle )
	:m_owner(renderer)
	,m_handle(handle)
{
}

//////////////////////////////////////////////////////////////////////////
SwapChain::~SwapChain()
{
	delete m_backbuffer;
	m_backbuffer = nullptr;

	m_owner = nullptr;
	DX_SAFE_RELEASE( m_handle );
}

//////////////////////////////////////////////////////////////////////////
void SwapChain::Present( int vsync /*= 0 */ )
{
	UNUSED( vsync );

	m_handle->Present( 0, 0 );
}

//////////////////////////////////////////////////////////////////////////
Texture* SwapChain::GetBackBuffer()
{
	if( nullptr != m_backbuffer )
	{
		return m_backbuffer;
	}

	ID3D11Texture2D* texHandle = nullptr;
	m_handle->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&texHandle );

	m_backbuffer = new Texture( m_owner, texHandle );

	return m_backbuffer;
}
