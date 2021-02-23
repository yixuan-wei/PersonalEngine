#pragma once

struct ID3D11RenderTargetView;
struct ID3D11ColorTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11Resource;
class Texture;

class TextureView
{
	friend class Texture;

public:
	TextureView();
	~TextureView();

	ID3D11RenderTargetView* GetRTVHandle() const { return m_rtv; }
	ID3D11ShaderResourceView* GetSRVHandle() const { return m_srv; }
	ID3D11DepthStencilView* GetDSVHandle() const { return m_dsv; }

private:
	Texture* m_owner = nullptr;

	union
	{
		ID3D11Resource* m_handle;        // A01, base for rtv and srv
		ID3D11RenderTargetView* m_rtv;
		ID3D11ShaderResourceView* m_srv; // A03 - what is bound to a shader stage for reading
		ID3D11DepthStencilView* m_dsv;
	};

};