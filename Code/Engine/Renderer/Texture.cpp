#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"

#pragma warning(push)
#pragma warning(disable:4100)
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"
#pragma  warning(pop)


//////////////////////////////////////////////////////////////////////////
Texture* Texture::CreateTextureFromFile(RenderContext* ctx, char const* filePath, bool flipVertical)
{
    int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
    int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
    int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
    int numComponentsRequested = 4; // only support 4
    stbi_set_flip_vertically_on_load((int)flipVertical); // not flipping
    unsigned char* imageData = stbi_load(filePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested);

    // Check if the load was successful
    if (imageData == nullptr || imageTexelSizeX <= 0 || imageTexelSizeY <= 0) {
        g_theConsole->PrintString(Rgba8::RED, Stringf("picture %s reading failed", filePath));
        return nullptr;
    }
    if (numComponents < 3 || numComponents>4) {
        g_theConsole->PrintString(Rgba8::RED, Stringf("picture %s has %i channels", filePath, numComponents));
    }

    //DirectX create
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = imageTexelSizeX;
    desc.Height = imageTexelSizeY;
    desc.MipLevels = 1;//2^ dimension, smoothed
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE;//if mip-chains, to be GPU/DEFAULT, Default for depth
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;//OR render target, Depth_Densile_Resource for depth, OR Shader Resource
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = imageData;
    initialData.SysMemPitch = imageTexelSizeX * 4;//continuous could be 0
    initialData.SysMemSlicePitch = 0;

    ID3D11Texture2D* texHandle = nullptr;
    ctx->m_device->CreateTexture2D(&desc, &initialData, &texHandle);

    // Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
    stbi_image_free(imageData);

    return new Texture(ctx, texHandle, filePath);
}

//////////////////////////////////////////////////////////////////////////
Texture* Texture::CreateDepthStencilTexture(RenderContext* ctx, IntVec2 const& outputSize)
{
    int totalNum = outputSize.x * outputSize.y;
    float* depthContent = new float[totalNum];
    for (int idx = 0; idx < totalNum; idx++) {
        depthContent[idx] = 0.f;
    }

    //DirectX create
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = outputSize.x;
    desc.Height = outputSize.y;
    desc.MipLevels = 1;//2^ dimension, smoothed
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;//if mip-chains, to be GPU/DEFAULT, Default for depth
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;//OR render target, Depth_Densile_Resource for depth, OR Shader Resource
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = depthContent;
    initialData.SysMemPitch = outputSize.x * sizeof(float);//continuous could be 0
    initialData.SysMemSlicePitch = 0;

    ID3D11Texture2D* texHandle = nullptr;
    ctx->m_device->CreateTexture2D(&desc, NULL, &texHandle);

    delete[] depthContent;
    
    return new Texture(ctx, texHandle, "depthDencil");
}

//////////////////////////////////////////////////////////////////////////
Texture::Texture( RenderContext* ctx, ID3D11Texture2D* handle, char const* filename )
    : m_owner(ctx)
    , m_handle(handle)
	, m_filePath(filename)
{
	D3D11_TEXTURE2D_DESC desc;
	m_handle->GetDesc( &desc );

	m_size = IntVec2( desc.Width, desc.Height );
}

Texture::Texture(RenderContext* ctx, ID3D11Texture2D* handle)
    : m_owner(ctx)
    , m_handle(handle)
{
    D3D11_TEXTURE2D_DESC desc;
    m_handle->GetDesc(&desc);

    m_size = IntVec2(desc.Width, desc.Height);
}

//////////////////////////////////////////////////////////////////////////
Texture::~Texture()
{
	delete m_renderTargetView;
	m_renderTargetView = nullptr;

	delete m_shaderResourceView;
	m_shaderResourceView = nullptr;

	delete m_depthStencilView;
	m_depthStencilView = nullptr;

	m_owner = nullptr;
	DX_SAFE_RELEASE( m_handle );
}

//////////////////////////////////////////////////////////////////////////
void Texture::SetTextureName(char const* newFilePath)
{
    m_filePath = newFilePath;
}

//////////////////////////////////////////////////////////////////////////
TextureView* Texture::GetOrCreateRenderTargetView()
{
	if( m_renderTargetView )
	{
		return m_renderTargetView;
	}

	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11Device* dev = m_owner->m_device;
	dev->CreateRenderTargetView( m_handle, nullptr, &rtv );

	if( rtv != nullptr )
	{
		m_renderTargetView = new TextureView();
		m_renderTargetView->m_rtv = rtv;
	}
	
	return m_renderTargetView;
}

//////////////////////////////////////////////////////////////////////////
TextureView* Texture::GetOrCreateShaderResourceView()
{
	if (m_shaderResourceView)
	{
		return m_shaderResourceView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;

	dev->CreateShaderResourceView(m_handle, nullptr, &srv);
	if (srv != nullptr)
	{
		m_shaderResourceView = new TextureView();
		m_shaderResourceView->m_srv = srv;
	}

	return m_shaderResourceView;
}

//////////////////////////////////////////////////////////////////////////
TextureView* Texture::GetOrCreateDepthStencilView()
{
	if (m_depthStencilView) {
		return m_depthStencilView;
	}

    ID3D11Device* dev = m_owner->m_device;
    ID3D11DepthStencilView* dsv = nullptr;

    D3D11_DEPTH_STENCIL_VIEW_DESC desc;
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;
    desc.Flags = 0;

    dev->CreateDepthStencilView(m_handle, &desc, &dsv);
    if (dsv != nullptr)
    {
        m_depthStencilView = new TextureView();
        m_depthStencilView->m_dsv = dsv;
    }

    return m_depthStencilView;
}


