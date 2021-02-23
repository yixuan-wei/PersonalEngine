#pragma once

#include "Engine/Math/IntVec2.hpp"
#include <string>

struct ID3D11Texture2D;
struct ID3D11Resource;
class RenderContext;
class TextureView;

class Texture
{
public:
	static Texture* CreateTextureFromFile(RenderContext* ctx, char const* filePath, bool flipVertical=true);
	static Texture* CreateDepthStencilTexture(RenderContext* ctx, IntVec2 const& outputSize);

	explicit Texture(RenderContext* ctx, ID3D11Texture2D* handle );
	explicit Texture( RenderContext* ctx, ID3D11Texture2D* handle, char const* filename );
	~Texture();	

	void SetTextureName(char const* newFilePath);

	TextureView* GetOrCreateRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();
	TextureView* GetOrCreateDepthStencilView();

	std::string const& GetTexturePath() const { return m_filePath; }
	const IntVec2 GetTextureSize() const { return m_size; }
	ID3D11Resource* GetHandle() const { return m_resource; }

private: 
	IntVec2      m_size;
	std::string  m_filePath;

	RenderContext* m_owner = nullptr;

	TextureView* m_renderTargetView = nullptr;
	TextureView* m_shaderResourceView = nullptr;
	TextureView* m_depthStencilView = nullptr;
	union{ 
		ID3D11Resource* m_resource; 
		ID3D11Texture2D* m_handle;
	};
};