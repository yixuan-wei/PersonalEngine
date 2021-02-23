#pragma once

class RenderContext;
class Texture;
struct IDXGISwapChain;

class SwapChain
{
public:
	SwapChain( RenderContext* renderer, IDXGISwapChain* handle );
	~SwapChain();

	void Present( int vsync = 0 );

	Texture* GetBackBuffer();

public:
	RenderContext* m_owner = nullptr; // creator context
	IDXGISwapChain* m_handle = nullptr; //D3D11 created pointer, used when calling D3D11 calls

	Texture* m_backbuffer = nullptr;
};