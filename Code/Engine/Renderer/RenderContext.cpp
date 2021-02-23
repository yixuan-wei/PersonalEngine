#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

#pragma comment( lib, "d3d11.lib" )         // needed a01
#pragma comment( lib, "dxgi.lib" )          // needed a01
#pragma comment( lib, "d3dcompiler.lib" )   // needed when we get to shaders

//////////////////////////////////////////////////////////////////////////
void RenderContext::Startup(Window* window)
{
	IDXGISwapChain* swapchain;

	UINT flags = 0; D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(RENDER_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	memset( &swapchainDesc, 0, sizeof( swapchainDesc ) );

	// how many back buffers in our chain - we'll double buffer (one we show, one we draw to)
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // on swap, the old buffer is discarded
	swapchainDesc.Flags = 0; // additional flags - see docs.  Used in special cases like for video buffers

	// how swap chain is to be used
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.OutputWindow = (HWND)window->m_hwnd; // HWND for the window to be used
	swapchainDesc.SampleDesc.Count = 1; // how many samples per pixel (1 so no MSAA)
										 // note, if we're doing MSAA, we'll do it on a secondary target
	// describe the buffer
	swapchainDesc.Windowed = TRUE;                                    // windowed/full-screen mode
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color RGBA8 color
	swapchainDesc.BufferDesc.Width = window->GetClientWidth();
	swapchainDesc.BufferDesc.Height = window->GetClientHeight();

	//create
	HRESULT result = D3D11CreateDeviceAndSwapChain( nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,//controls the type of device we make
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapchainDesc,
		&swapchain,
		&m_device,
		nullptr,
		&m_context );

	GUARANTEE_OR_DIE( SUCCEEDED( result ), "failed to create rendering device" );

	m_swapchain = new SwapChain( this, swapchain );
	Texture* backbuffer = m_swapchain->GetBackBuffer();
	m_defaultDepthBuffer = Texture::CreateDepthStencilTexture(this, backbuffer->GetTextureSize());

	m_defaultShader = Shader::CreateDefaultShader(this);
	m_errorShader = Shader::CreateErrorShader(this);

	m_gameClock = new Clock();
	m_effectCamera = new Camera();

	m_immediateMesh = new GPUMesh(this);

	m_modelUBO    = new RenderBuffer(this, eRenderBufferUsageBit::UNIFORM_BUFFER_BIT, eRenderMemoryHint::MEMORY_HINT_DYNAMIC);
	m_frameUBO    = new RenderBuffer(this, eRenderBufferUsageBit::UNIFORM_BUFFER_BIT, eRenderMemoryHint::MEMORY_HINT_DYNAMIC);
	m_lightUBO    = new RenderBuffer(this, eRenderBufferUsageBit::UNIFORM_BUFFER_BIT, eRenderMemoryHint::MEMORY_HINT_DYNAMIC);

	m_pointSampler = new Sampler(this, eSamplerType::SAMPLER_POINT);
	m_linearSampler = new Sampler(this, eSamplerType::SAMPLER_BILINEAR);
	m_texNormal = CreateDefaultNormalTexture();

	CreateDefaultColorTextures();
	CreateDefaultRasterizeState();
	CreateBlendStates();
	CreateDefaultShaderState();
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BeginFrame()
{
	UpdateFrameData();
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::EndFrame()
{
	m_swapchain->Present( );
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::Shutdown()
{
	for (size_t idx = 0; idx < m_renderTargetPool.size(); idx++) {
		if (m_renderTargetPool[idx] != nullptr) {
			delete m_renderTargetPool[idx];
		}
	}

    for (size_t idx = 0; idx < m_loadedTextures.size(); idx++)
    {
        if (m_loadedTextures[idx] != nullptr)
        {
            delete m_loadedTextures[idx];
            m_loadedTextures[idx] = nullptr;
        }
    }

    for (std::map<std::string, Shader*>::iterator iter = m_loadedShaders.begin(); iter != m_loadedShaders.end(); iter++) {
        if (iter->second != nullptr && iter->second != m_errorShader)
        {
            delete iter->second;
            iter->second = nullptr;
        }
    }

    for (size_t idx = 0; idx < m_loadedFonts.size(); idx++)
    {
        if (m_loadedFonts[idx] != nullptr)
        {
            delete m_loadedFonts[idx];
            m_loadedFonts[idx] = nullptr;
        }
    }

	delete m_defaultShader;
	m_defaultShader = nullptr;

	delete m_defaultDepthBuffer;
	m_defaultDepthBuffer = nullptr;

	delete m_errorShader;
	m_errorShader = nullptr;

	delete m_pointSampler;
	m_pointSampler = nullptr;

	delete m_linearSampler;
	m_linearSampler = nullptr;

	delete m_modelUBO;
	m_modelUBO = nullptr;

	delete m_frameUBO;
	m_frameUBO = nullptr;

	delete m_lightUBO;
	m_lightUBO = nullptr;

	delete m_immediateMesh;
	m_immediateMesh = nullptr;

	delete m_effectCamera;
	m_effectCamera = nullptr;

	delete m_swapchain;
	m_swapchain = nullptr;
	
    /*ID3D11Debug* pDebug;
    HRESULT hr = m_device->QueryInterface(IID_PPV_ARGS(&pDebug));*/

    DX_SAFE_RELEASE(m_defaultRasterState);
    DX_SAFE_RELEASE(m_tempRasterState);
    DX_SAFE_RELEASE(m_currentDepthStencilState);
    DX_SAFE_RELEASE(m_additiveBlendStateHandle);
    DX_SAFE_RELEASE(m_alphaBlendStateHandle);
    DX_SAFE_RELEASE(m_opaqueBlendStateHandle);
    DX_SAFE_RELEASE(m_context);
    DX_SAFE_RELEASE(m_device);

    /*if (pDebug) {
        pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    }

    DX_SAFE_RELEASE(pDebug);*/
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetupParentClock(Clock* gameClock)
{
	m_gameClock->SetParent(gameClock);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetGamma(float newGamma)
{
	m_gamma = newGamma < 1.f ? 1.f:newGamma;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::UpdateFrameData()
{
    frame_data_t frameData;
	frameData.system_time = (float)Clock::GetMaster()->GetTotalElapsedSeconds();
    frameData.system_delta_time = (float)m_gameClock->GetLastDeltaSeconds();
	frameData.gamma = m_gamma;

    m_frameUBO->Update(&frameData, sizeof(frameData), sizeof(frameData));
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::UpdateLightData()
{
	if (m_lightDirty) {
		m_lightUBO->Update(&m_lightData, sizeof(m_lightData), sizeof(m_lightData));
		m_lightDirty = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::ClearRenderTargetView( const Rgba8& clearColor )// Clear all screen (back buffer) pixels to "clearColor"
{
    float clearFloats[4];
    clearFloats[0] = (float)clearColor.r / 255.f;
    clearFloats[1] = (float)clearColor.g / 255.f;
    clearFloats[2] = (float)clearColor.b / 255.f;
    clearFloats[3] = (float)clearColor.a / 255.f;

	TextureView* rtv = GetFrameColorTarget()->GetOrCreateRenderTargetView();
	ClearRenderTargetView(rtv->GetRTVHandle(), clearFloats);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::ClearRenderTargetView(ID3D11RenderTargetView* rtv, float const* clearFloats)
{
    m_context->ClearRenderTargetView(rtv, clearFloats);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::ClearDepth(Texture* depthStencilTexture, float depth)
{
	TextureView* view = depthStencilTexture->GetOrCreateDepthStencilView();
	ID3D11DepthStencilView* dsv = view->GetDSVHandle();
	m_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, depth, 0);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BeginCamera( Camera const* camera )// Establish a 2D (orthographic) drawing coordinate system according to Camera Class
{
#if defined(RENDER_DEBUG)
	m_context->ClearState();
	m_lastVBH = nullptr;
	m_currentShader = nullptr;
#endif

	//copy to effect camera
	if (camera != m_effectCamera) {
		m_effectCamera->SetProjectionMatrix(camera->GetProjectionMatrix());
		m_effectCamera->SetViewMatrix(camera->GetView());
	}

	m_currentCam = const_cast<Camera*>(camera);
	m_currentCam->UpdateCameraData(this);

	IntVec2 outputSize;
	std::vector<ID3D11RenderTargetView*> rtvs;
	int rtvCount = m_currentCam->GetColorTargetCount();
	rtvs.resize((size_t)rtvCount);
	for (size_t i = 0; i < rtvs.size(); i++) {
		rtvs[i] = nullptr;
		Texture* colorTarget = m_currentCam->GetColorTarget((int)i);
		if (colorTarget != nullptr) {
			outputSize = colorTarget->GetTextureSize();
			TextureView* rtv = colorTarget->GetOrCreateRenderTargetView();
			rtvs[i] = rtv->GetRTVHandle();
		}
	}
    if (rtvs[0] == nullptr) //fall back for sd1
    {
		outputSize = GetFrameColorTarget()->GetTextureSize();
        rtvs[0] = GetFrameColorTarget()->GetOrCreateRenderTargetView()->GetRTVHandle();
    }

	unsigned int clearMode = m_currentCam->GetClearMode();
    
	//get depth
	Texture* dsTex = m_currentCam->GetDepthStencilTarget();
	ID3D11DepthStencilView* dsView = nullptr;
    if (dsTex == nullptr) {
        dsTex = m_defaultDepthBuffer;
        dsView = dsTex->GetOrCreateDepthStencilView()->GetDSVHandle();
    }

	//clear screen & depth
	if (clearMode & CLEAR_COLOR_BIT) {
		Vec4 clearColor = m_currentCam->GetClearColor().ToFloats();
		float clearFloats[] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
		for (int i = 0; i < rtvCount; i++) {
			ClearRenderTargetView(rtvs[i], clearFloats);
		}
	}
	if (clearMode & CLEAR_DEPTH_BIT) {
		//when camera depth buffer not qualified
		ClearDepth(dsTex, m_currentCam->GetClearDepth());
		dsView = dsTex->GetOrCreateDepthStencilView()->GetDSVHandle();
	}

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)outputSize.x;
    viewport.Height = (float)outputSize.y;
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;

    m_context->OMSetRenderTargets(rtvCount, rtvs.data(), dsView);
    m_context->RSSetViewports(1, &viewport);
	m_context->RSSetState(m_defaultRasterState);

	BindDiffuseTexture((Texture*)nullptr);
	BindNormalTexture((Texture*)nullptr);
	BindSpecularTexture((Texture*)nullptr);
	BindEmissiveTexture((Texture*)nullptr);
	BindSampler(nullptr);
    BindShaderState((ShaderState*)nullptr);

	BindUniformBuffer(UBO_FRAME_SLOT, m_frameUBO);
	BindUniformBuffer(UBO_CAMERA_SLOT, m_currentCam->GetCameraUBO());
	BindUniformBuffer(UBO_OBJECT_SLOT, m_modelUBO);
	BindUniformBuffer(UBO_LIGHT_SLOT, m_lightUBO);

	SetModelData(Mat44::IDENTITY, Rgba8::WHITE);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::EndCamera( Camera const* camera)
{
	m_currentCam = nullptr;
	UNUSED(camera);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindIndexBuffer(IndexBuffer* ibo)
{
	ID3D11Buffer* iboHandle = ibo->GetHandle();
	m_context->IASetIndexBuffer(iboHandle, DXGI_FORMAT_R32_UINT, 0);

}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindVertexBuffer( VertexBuffer* vbo )
{
	if ( vbo->GetHandle() == m_lastVBH )
	{
		return;
	}

	UINT stride = (UINT)vbo->m_stride;
	UINT offset = 0;
	ID3D11Buffer* vboHandle = vbo->GetHandle();
	m_context->IASetVertexBuffers( 0, 1, &vboHandle, &stride, &offset );
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_currentLayout = vbo->m_attributes;
	m_lastVBH = vboHandle;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindUniformBuffer(unsigned int slot, RenderBuffer* ubo)
{
	ID3D11Buffer* uboHandle = ubo->GetHandle();
	m_context->VSSetConstantBuffers(slot, 1, &uboHandle);
	m_context->PSSetConstantBuffers(slot, 1, &uboHandle);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::ReloadShaders()
{
	for (std::map<std::string, Shader*>::iterator iter = m_loadedShaders.begin(); iter != m_loadedShaders.end(); iter++) {
		if (iter->second != m_errorShader) {
			delete iter->second;
		}
        Shader* shader = new Shader(this);
        if (shader->CreateFromFile(iter->first)) {
            iter->second = shader;
        }
        else {
            delete shader;
            g_theConsole->PrintError(Stringf("Fail to load %s", iter->first.c_str()));
            iter->second = m_errorShader;
        }
		
	}
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindMaterial(Material* mat)
{
	BindShaderState(mat->m_shaderState);

	for (unsigned int i = 0;i<(unsigned int)mat->m_userTextures.size();i++) {
		BindUserTexture(i, mat->m_userTextures[i]);
	}

    for (unsigned int i = 0; i < (unsigned int)mat->m_userSamplers.size(); i++) {
        BindUserSampler(i, mat->m_userSamplers[i]);
    }

	mat->UpdateUBO(this);
	SetMaterialBuffer(0, mat->m_ubo);
	SetModelTint(mat->m_tint);
	SetSpecularFactors(mat->m_specularFactor, mat->m_specularPower);
	
	BindDiffuseTexture(mat->m_diffuseTexture);
	BindNormalTexture(mat->m_normalTexture);
	BindSpecularTexture(mat->m_specularTexture);
	BindEmissiveTexture(mat->m_emissiveTexture);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindShaderState(ShaderState const* shaderState)
{
	ShaderState const* state = shaderState;
	if (state == nullptr) {
		state = m_defaultShaderState;
	}
	BindShader(state->m_shader);
	SetBlendMode(state->m_blendMode);
	EnableDepth(state->m_depthCompare, state->m_writeDepth);
	SetFrontFaceWindOrder(state->m_windOrderCounterClockwise);
	SetCullMode(state->m_cullMode);
	SetFillMode(state->m_fillMode);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindShaderStateByName(char const* filename)
{
	ShaderState* state = CreateOrGetShaderState(filename);
	BindShaderState(state);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindShader( Shader* shader )
{
	ASSERT_OR_DIE(m_currentCam!=nullptr,"BindShader called outside of begin/end camera");

	if (m_currentShader!=nullptr && m_currentShader == shader) {
		return;
	}

	m_shaderHasChanged = true;
	m_currentShader = shader;
	if( m_currentShader == nullptr )
	{
		m_currentShader = m_defaultShader;
	}

	m_context->VSSetShader(m_currentShader->m_vertexStage.m_vs, nullptr, 0);
	m_context->PSSetShader(m_currentShader->m_fragmentStage.m_fs, nullptr, 0);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindUserSampler(unsigned int slot, Sampler const* sampler)
{
	ID3D11SamplerState* handle = nullptr;
	if (sampler == nullptr)	{
		handle = m_pointSampler->GetHandle();
	}
	else {
		handle = sampler->GetHandle();
	}
	
	m_context->PSSetSamplers(slot+1,1,&handle);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindSampler(Sampler const* sampler)
{
    ID3D11SamplerState* handle = nullptr;
    if (sampler == nullptr) {
        handle = m_pointSampler->GetHandle();
    }
    else {
        handle = sampler->GetHandle();
    }

    m_context->PSSetSamplers(0, 1, &handle);
}

//////////////////////////////////////////////////////////////////////////
Shader* RenderContext::CreateOrGetShader(char const* filename)
{
	//search in loaded shaders
	for (std::map<std::string, Shader*>::const_iterator iter = m_loadedShaders.begin(); iter != m_loadedShaders.end();iter++) {
		if (iter->first == filename)
		{
			return iter->second;
		}
	}

	//new a shader
	Shader* shader = new Shader(this);
	if (shader->CreateFromFile(filename))
	{
		m_loadedShaders[filename] = shader;
		return shader;
	}
	else { // creation failed
		delete shader;
		g_theConsole->PrintError(Stringf("Fail to load %s", filename));
		m_loadedShaders[filename] = m_errorShader;
        return m_errorShader;
	}
}

//////////////////////////////////////////////////////////////////////////
//Given number of vertexes and vertexes, draw them using OpenGL triangles.
//thus number of vertexes must be multiply of 3
void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	m_immediateMesh->UpdateVertices(numVertexes, vertexes);
	m_immediateMesh->UpdateIndices(0, nullptr);

	DrawMesh(m_immediateMesh);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexArray )
{
	if (vertexArray.size() < 1)
	{
		return;
	}

	DrawVertexArray( static_cast<int>(vertexArray.size()), &vertexArray[0] );
}

//////////////////////////////////////////////////////////////////////////
Texture* RenderContext::CreateOrGetTextureFromFile(const char* filePath, bool reload/*false*/, bool flipVertical/*true*/)
{	
	if(!reload){
        for (int textureID = 0; textureID < static_cast<int>(m_loadedTextures.size()); textureID++)
        {
            if (m_loadedTextures[textureID]->GetTexturePath() == filePath)
            {
                return m_loadedTextures[textureID];
            }
        }
	}
	else {
        for (int textureID = 0; textureID < static_cast<int>(m_loadedTextures.size()); textureID++)
        {
			Texture* tex = m_loadedTextures[textureID];
            if (tex->GetTexturePath() == filePath)
            {
                delete tex;
				m_loadedTextures.erase(m_loadedTextures.begin()+textureID);
				break;
            }
        }
	}

	if( CreateTextureFromFile( filePath, flipVertical ) )
		return m_loadedTextures[m_loadedTextures.size() - 1];
	else 
		return nullptr;
}

//////////////////////////////////////////////////////////////////////////
BitmapFont* RenderContext::CreateOrGetBitmapFont( const char* filePathNoExtension )
{
	std::string realPath = filePathNoExtension;
	realPath += ".png";
	for( int fontID = 0; fontID < (int)m_loadedFonts.size(); fontID++ )
	{
		if( m_loadedFonts[fontID]->GetFontPath() == realPath )
			return m_loadedFonts[fontID];
	}

	return CreateBitmapFontFromFile( realPath.c_str() );
}

//////////////////////////////////////////////////////////////////////////
BitmapFont* RenderContext::CreateBitmapFontFromFile( const char* filePath )
{
	Texture* fontTexture = CreateOrGetTextureFromFile( filePath );
	BitmapFont* font = new BitmapFont( filePath, fontTexture );
	m_loadedFonts.push_back( font );
	return font;
}

//////////////////////////////////////////////////////////////////////////
ShaderState* RenderContext::CreateShaderStateFromFile(char const* filePath)
{
	XmlDocument shaderStateDoc;
	XmlError code = shaderStateDoc.LoadFile(filePath);
	GUARANTEE_OR_DIE(code == XmlError::XML_SUCCESS, Stringf("Error when loading %s", filePath));

	ShaderState* newState = new ShaderState(this);
	newState->SetupFromXML(*shaderStateDoc.RootElement());
	m_loadedShaderStates[filePath] = newState;
	return newState;
}

//////////////////////////////////////////////////////////////////////////
Texture* RenderContext::CreateDefaultNormalTexture()
{
    //make a 1x1 texture of z+ normal
    int imageTexelSizeX = 1; // This will be filled in for us to indicate image width
    int imageTexelSizeY = 1; // This will be filled in for us to indicate image height

    float imageData[] = { 0.5f, 0.5f, 1.0f };

    //DirectX create
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = imageTexelSizeX;
    desc.Height = imageTexelSizeY;
    desc.MipLevels = 1;//2^ dimension, smoothed
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE;//if mip-chains, to be GPU/DEFAULT
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;//OR render target
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = imageData;
    initialData.SysMemPitch = imageTexelSizeX * 3 * sizeof(float);//continuous could be 0
    initialData.SysMemSlicePitch = 0;

    ID3D11Texture2D* texHandle = nullptr;
    m_device->CreateTexture2D(&desc, &initialData, &texHandle);

    Texture* newTexture = new Texture(this, texHandle, "Flat");
	m_loadedTextures.push_back(newTexture);
    return newTexture;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::CreateDefaultColorTextures()
{
	m_texWhite = CreateTextureFromColor(Rgba8::WHITE);
	m_texWhite->SetTextureName("White");
	m_loadedTextures.push_back(m_texWhite);

	m_texBlack = CreateTextureFromColor(Rgba8::BLACK);
	m_texBlack->SetTextureName("Black");
	m_loadedTextures.push_back(m_texBlack);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::UpdateInputLayout()
{
     //Describe Vertex format to shader
    if ((m_lastLayout!=m_currentLayout) || m_shaderHasChanged)
    {
		ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout(m_currentLayout); 
        m_context->IASetInputLayout(inputLayout);

		m_shaderHasChanged = false;
		m_lastLayout = m_currentLayout;
    }
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindDiffuseTexture( Texture const* constTex )
{
	Texture* texture = nullptr;
	if (constTex == nullptr)
	{
		texture = m_texWhite;
	}
	else {
		texture = const_cast<Texture*>(constTex);
	}
	//always bind, at least white texture
	TextureView* srv = texture->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* shaderResourceView = srv->GetSRVHandle();
	m_context->PSSetShaderResources((UINT)TEX_DIFFUSE_SLOT, 1, &shaderResourceView);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindDiffuseTexture(char const* filename)
{
	Texture* texture = CreateOrGetTextureFromFile(filename);
	BindDiffuseTexture(texture);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindNormalTexture(Texture const* constTex)
{
    Texture* texture = nullptr;
    if (constTex == nullptr)
    {
        texture = m_texNormal;
    }
    else {
        texture = const_cast<Texture*>(constTex);
    }
    //always bind, at least white texture
    TextureView* srv = texture->GetOrCreateShaderResourceView();
    ID3D11ShaderResourceView* shaderResourceView = srv->GetSRVHandle();
    m_context->PSSetShaderResources((UINT)TEX_NORMAL_SLOT, 1, &shaderResourceView);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindNormalTexture(char const* filename)
{
	Texture* texture = CreateOrGetTextureFromFile(filename);
	BindNormalTexture(texture);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindSpecularTexture(char const* filename)
{
    Texture* texture = CreateOrGetTextureFromFile(filename);
    BindSpecularTexture(texture);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindSpecularTexture(Texture const* constTex)
{
    Texture* texture = nullptr;
    if (constTex == nullptr)
    {
        texture = m_texWhite;
    }
    else {
        texture = const_cast<Texture*>(constTex);
    }
    //always bind, at least white texture
    TextureView* srv = texture->GetOrCreateShaderResourceView();
    ID3D11ShaderResourceView* shaderResourceView = srv->GetSRVHandle();
    m_context->PSSetShaderResources((UINT)TEX_SPEC_SLOT, 1, &shaderResourceView);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindEmissiveTexture(Texture const* constTex)
{
    Texture* texture = nullptr;
    if (constTex == nullptr)
    {
        texture = m_texBlack;
    }
    else {
        texture = const_cast<Texture*>(constTex);
    }
    //always bind, at least white texture
    TextureView* srv = texture->GetOrCreateShaderResourceView();
    ID3D11ShaderResourceView* shaderResourceView = srv->GetSRVHandle();
    m_context->PSSetShaderResources((UINT)TEX_EMISSIVE_SLOT, 1, &shaderResourceView);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindEmissiveTexture(char const* filename)
{
    Texture* texture = CreateOrGetTextureFromFile(filename);
    BindEmissiveTexture(texture);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindUserTexture(unsigned int idx, Texture const* constTex)
{
    Texture* texture = nullptr;
    if (constTex == nullptr)
    {
        texture = m_texWhite;
    }
    else {
        texture = const_cast<Texture*>(constTex);
    }
    //always bind, at least white texture
    TextureView* srv = texture->GetOrCreateShaderResourceView();
    ID3D11ShaderResourceView* shaderResourceView = srv->GetSRVHandle();
    m_context->PSSetShaderResources((unsigned int)TEX_USER_START_SLOT+idx, 1, &shaderResourceView);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::BindUserTexture(unsigned int idx, char const* filename)
{
	Texture* texture = CreateOrGetTextureFromFile(filename);
	BindUserTexture(idx, texture);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::EnableDepth(eCompareFunc func, bool writeDepthOnPass)
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthFunc = ToDXCompareFunc(func);
	desc.DepthEnable = true;
	desc.DepthWriteMask = writeDepthOnPass?D3D11_DEPTH_WRITE_MASK_ALL:D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.StencilEnable = FALSE;

	DX_SAFE_RELEASE(m_currentDepthStencilState);

	m_device->CreateDepthStencilState(&desc, &m_currentDepthStencilState);
	m_context->OMSetDepthStencilState(m_currentDepthStencilState, 0);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DisableDepth()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.StencilEnable = false;

    DX_SAFE_RELEASE(m_currentDepthStencilState);

    m_device->CreateDepthStencilState(&desc, &m_currentDepthStencilState);
    m_context->OMSetDepthStencilState(m_currentDepthStencilState, 0);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetCullMode(eCullMode cullMode)
{
	DX_SAFE_RELEASE(m_tempRasterState);

    D3D11_RASTERIZER_DESC desc;
    m_context->RSGetState(&m_tempRasterState);
    m_tempRasterState->GetDesc(&desc);
	D3D11_CULL_MODE mode = ToDXCullMode(cullMode);
	if (mode != desc.CullMode) {
		DX_SAFE_RELEASE(m_tempRasterState);

		desc.CullMode = mode;
		m_device->CreateRasterizerState(&desc, &m_tempRasterState);
		m_context->RSSetState(m_tempRasterState);
	}
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetFillMode(eFillMode fillMode)
{    
	DX_SAFE_RELEASE(m_tempRasterState);

	D3D11_RASTERIZER_DESC desc;
	m_context->RSGetState(&m_tempRasterState);
	m_tempRasterState->GetDesc(&desc);
	D3D11_FILL_MODE mode = ToDXFillMode(fillMode);
	if (mode != desc.FillMode) {
		DX_SAFE_RELEASE(m_tempRasterState);

		desc.FillMode = mode;
		m_device->CreateRasterizerState(&desc, &m_tempRasterState);
		m_context->RSSetState(m_tempRasterState);
	}
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetFrontFaceWindOrder(bool isCounterClockwise)
{
	DX_SAFE_RELEASE(m_tempRasterState);

	D3D11_RASTERIZER_DESC desc;
	m_context->RSGetState(&m_tempRasterState);
	m_tempRasterState->GetDesc(&desc);
	if (desc.FrontCounterClockwise != (BOOL)isCounterClockwise) {
		DX_SAFE_RELEASE(m_tempRasterState);

		desc.FrontCounterClockwise = isCounterClockwise;
		m_device->CreateRasterizerState(&desc, &m_tempRasterState);
		m_context->RSSetState(m_tempRasterState);
	}
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetBlendMode( eBlendMode blendmode )
{
    float const zeroes[] = { 0.f,0.f,0.f,0.f };

	switch (blendmode)
	{
	case eBlendMode::BLEND_ALPHA:	m_context->OMSetBlendState(m_alphaBlendStateHandle, zeroes, 0xffffffff);		break;
	case eBlendMode::BLEND_ADDITIVE:	m_context->OMSetBlendState(m_additiveBlendStateHandle, zeroes, 0xffffffff);		break;
	case eBlendMode::BLEND_OPAQUE:	m_context->OMSetBlendState(m_opaqueBlendStateHandle, zeroes, 0xffffffff);		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetModelMatrix(Mat44 const& newModelMatrix)
{
	m_objectData.model = newModelMatrix;
	m_modelUBO->Update(&m_objectData, sizeof(m_objectData), sizeof(m_objectData));
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetModelTint(Rgba8 const& newTint)
{
	m_objectData.tint = newTint.ToFloats();
	m_modelUBO->Update(&m_objectData, sizeof(m_objectData), sizeof(m_objectData));
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetModelData(Mat44 const& newModelMatrix, Rgba8 const& newTint)
{
	m_objectData.model = newModelMatrix;
	m_objectData.tint = newTint.ToFloats();

	m_modelUBO->Update(&m_objectData, sizeof(m_objectData), sizeof(m_objectData));
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetMaterialBuffer( unsigned int idx, RenderBuffer* buffer)
{
	BindUniformBuffer((unsigned int)UBO_MATERIAL_SLOT+idx, buffer);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::EnableFog(float nearFog, float farFog, Rgba8 const& fogColor)
{
	m_lightData.fogNear = nearFog;
	m_lightData.fogFar = farFog;
	m_lightData.fogColor = fogColor.ToFloats().GetXYZ();
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DisableFog()
{
	m_lightData.fogNear = m_lightData.fogFar = FLT_MAX;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::EnableLight(unsigned int idx, light_t const& lightInfo)
{
	if (idx >= MAX_LIGHT_COUNT) {
		return;
	}

	m_lightData.light[idx] = lightInfo;
	m_lightDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DisableLight(unsigned int idx)
{
	if (idx >= MAX_LIGHT_COUNT) {
		return;
	}

	m_lightData.light[idx].intensity = 0.f;
	m_lightDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetDiffuseFactor(float diffuseFactor /*= 1.f*/)
{
	m_lightData.diffuseFactor = diffuseFactor;
	m_lightDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetEmissiveFactor(float emissiveFactor /*= 1.f*/)
{
	m_lightDirty = true;
	m_lightData.emissiveFactor = emissiveFactor;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetSpecularFactors(float specularFactor /*= 1.f*/, float specularPower /*= 200.f*/)
{
	m_lightData.specularFactor = specularFactor;
	m_lightData.specularPower = specularPower;
	m_lightDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetAmbientColor(Rgba8 const& color)
{
    Vec4 tint = color.ToFloats();
    m_lightData.ambient = Vec4(tint.GetXYZ(), m_lightData.ambient.w);
	m_lightDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetAmbientIntensity(float intensity)
{
	m_lightData.ambient.w = intensity;
	m_lightDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::SetAmbientLight(Rgba8 const& color, float intensity)
{
	Vec4 tint = color.ToFloats();
	m_lightData.ambient = Vec4(tint.GetXYZ(), intensity);
	m_lightDirty = true;
}

//////////////////////////////////////////////////////////////////////////
float RenderContext::GetAmbientIntensity() const
{
	return m_lightData.ambient.w;
}

//////////////////////////////////////////////////////////////////////////
Texture* RenderContext::GetFrameColorTarget() const
{
	return m_swapchain->GetBackBuffer();
}

//////////////////////////////////////////////////////////////////////////
Texture* RenderContext::AcquireRenderTargetMatching(Texture* tex)
{
	IntVec2 size = tex->GetTextureSize();
	for (size_t i = 0; i < m_renderTargetPool.size(); i++) {
		Texture* rt = m_renderTargetPool[i];
		if (rt->GetTextureSize() == size) {
			m_renderTargetPool[i] = m_renderTargetPool[m_renderTargetPool.size() - 1];
			m_renderTargetPool.pop_back();

			return rt;
		}
	}

	//make a new one when can't find maching
	++m_totalRenderTargetMade;
	Texture* newRenderTarget = CreateRenderTarget(size);
	return newRenderTarget;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::ReleaseRenderTarget(Texture* tex)
{
	m_renderTargetPool.push_back(tex);
}

//////////////////////////////////////////////////////////////////////////
Texture* RenderContext::CreateRenderTarget(IntVec2 const& texelSize)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = texelSize.x;
	desc.Height = texelSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D(&desc, nullptr, &texHandle);

	Texture* newTexture = new Texture(this, texHandle, "");
	return newTexture;
}

//////////////////////////////////////////////////////////////////////////
int RenderContext::GetTotalRenderTargetCount() const
{
	return m_totalRenderTargetMade;
}

//////////////////////////////////////////////////////////////////////////
int RenderContext::GetFreeRenderTargetCount() const
{
	return (int)m_renderTargetPool.size();
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::ApplyEffect(Texture* dst, Texture* src, Material* mat)
{
	GUARANTEE_OR_DIE(m_currentCam == nullptr, "Apply Effect happens between begin/end cam");

	m_effectCamera->SetColorTarget(dst);

	BeginCamera(m_effectCamera);
	BindMaterial(mat);
	BindDiffuseTexture(src);

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->Draw(3, 0);

	EndCamera(m_effectCamera);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::CopyTexture(Texture* dst, Texture* src)
{
	m_context->CopyResource(dst->GetHandle(), src->GetHandle());
}

//////////////////////////////////////////////////////////////////////////
ShaderState* RenderContext::CreateOrGetShaderState(char const* filename)
{
    for (std::map<std::string, ShaderState*>::const_iterator iter = m_loadedShaderStates.begin(); iter != m_loadedShaderStates.end(); iter++) {
        if (iter->first == filename)
        {
            return iter->second;
        }
    }

	return CreateShaderStateFromFile(filename);
}

//////////////////////////////////////////////////////////////////////////
Texture* RenderContext::CreateTextureFromColor(Rgba8 color)
{
	//make a 1x1 texture of that color
    int imageTexelSizeX = 1; // This will be filled in for us to indicate image width
    int imageTexelSizeY = 1; // This will be filled in for us to indicate image height

	unsigned char imageData[] = { color.r,color.g,color.b,color.a };

    //DirectX create
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = imageTexelSizeX;
    desc.Height = imageTexelSizeY;
    desc.MipLevels = 1;//2^ dimension, smoothed
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE;//if mip-chains, to be GPU/DEFAULT
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;//OR render target
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = imageData;
    initialData.SysMemPitch = imageTexelSizeX * 4;//continuous could be 0
    initialData.SysMemSlicePitch = 0;

    ID3D11Texture2D* texHandle = nullptr;
    m_device->CreateTexture2D(&desc, &initialData, &texHandle);

    Texture* newTexture = new Texture(this, texHandle,"Color");
	return newTexture;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::Draw( int numVertexes, int vertexOffset /*= 0 */ )
{
	m_context->Draw( numVertexes, vertexOffset );
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawIndexed(int indexCount, int indexOffset /*= 0*/, int vertexOffset /*= 0*/)
{
	m_context->DrawIndexed(indexCount, indexOffset, vertexOffset);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawMesh(GPUMesh* mesh)
{
	if (mesh->m_vertices->m_count < 1) {
		return;
	}

	BindVertexBuffer(mesh->m_vertices);
	UpdateInputLayout();
	UpdateLightData();

	bool hasIndices = mesh->GetIndexCount() > 0;

	if (hasIndices) {
		BindIndexBuffer(mesh->m_indices);
		DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}
	else {
		Draw(mesh->GetVertexCount(), 0);
	}
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawLine2D( const Vec2& startPoint, const Vec2& endPoint, float thickness, const Rgba8& color)
{
	Vertex_PCU verts[6];

	Vec2 forward = endPoint - startPoint;
	forward = forward.GetNormalized();
	Vec2 vertical = forward.GetRotated90Degrees();
	float length = thickness * .5f;

	Vec2 endUp = endPoint + (forward + vertical) * length;
	Vec2 endDown = endPoint + (forward - vertical) * length;
	Vec2 startUp = startPoint + (vertical - forward) * length;
	Vec2 startDown = startPoint + (-vertical - forward) * length;

	//right down triangle
	verts[0] = Vertex_PCU( startDown, color );
	verts[1] = Vertex_PCU( endDown, color );
	verts[2] = Vertex_PCU( endUp, color );
	//left up triangle			   
	verts[3] = Vertex_PCU( startDown, color );
	verts[4] = Vertex_PCU( endUp, color );
	verts[5] = Vertex_PCU( startUp, color );

	DrawVertexArray( 6, verts );
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawDisc2D( const Vec2& position, float radius, const Rgba8& color, int fragmentNum /*= CIRCLE_FRAGMENT_NUM */ )
{
	Vertex_PCU* verts=new Vertex_PCU[3 * fragmentNum];

	float unitDegrees = 360.f / (float)CIRCLE_FRAGMENT_NUM;
	Vec2* ringVerts = new Vec2[CIRCLE_FRAGMENT_NUM];
	for( int vertID = 0; vertID < CIRCLE_FRAGMENT_NUM; vertID++ )
	{
		ringVerts[vertID] = position + Vec2::MakeFromPolarDegrees( unitDegrees * (float)vertID, radius );
	}

	for( int vertID = 0; vertID < fragmentNum; vertID++ )
	{
		int triangleID = 3 * vertID;
		verts[triangleID]= Vertex_PCU( position, color ) ;
		verts[triangleID+1]=( Vertex_PCU( ringVerts[vertID], color ) );
		if( vertID == CIRCLE_FRAGMENT_NUM - 1 )
		{
			verts[triangleID+2]=( Vertex_PCU( ringVerts[0], color ) );
		}
		else
		{
			verts[triangleID+2]=( Vertex_PCU( ringVerts[vertID + 1], color ) );
		}
	}

	DrawVertexArray( 3 * fragmentNum, verts );

	delete[] verts;
	delete[] ringVerts;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawSquare2D(Vec3 const& position, float width, Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvAtMins /*= Vec2::ZERO*/, Vec2 const& uvAtMaxs /*= Vec2::ONE*/)
{
	float halfWidth = width * .5f;
	Vec3 halfDim(halfWidth, halfWidth, 0.f);
    Vec3 mins = position - halfDim;
    Vec3 maxs = position + halfDim;
    Vertex_PCU bottomLeft = Vertex_PCU(mins, color, uvAtMins);
    Vertex_PCU upperRight = Vertex_PCU(maxs, color, uvAtMaxs);

    Vertex_PCU verts[6];
    verts[0] = (bottomLeft);
    verts[1] = (Vertex_PCU(Vec3(maxs.x, mins.y,position.z), color, Vec2(uvAtMaxs.x, uvAtMins.y)));
    verts[2] = (upperRight);
    verts[3] = (bottomLeft);
    verts[4] = (upperRight);
    verts[5] = (Vertex_PCU(Vec3(mins.x, maxs.y,position.z), color, Vec2(uvAtMins.x, uvAtMaxs.y)));

	DrawVertexArray(6, &verts[0]);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawAABB2D( const AABB2& bounds, const Rgba8& color , const Vec2& uvAtMins, const Vec2& uvAtMaxs)
{
	Vec2 mins = bounds.mins;
	Vec2 maxs = bounds.maxs;
	Vertex_PCU bottomLeft = Vertex_PCU( mins, color, uvAtMins );
	Vertex_PCU upperRight = Vertex_PCU( maxs, color, uvAtMaxs );

	Vertex_PCU verts[6];
	verts[0]=( bottomLeft );
	verts[1]=( Vertex_PCU( Vec2( maxs.x, mins.y ), color, Vec2( uvAtMaxs.x, uvAtMins.y ) ) );
	verts[2]=( upperRight );
	verts[3]=( bottomLeft );
	verts[4]=( upperRight );
	verts[5]=( Vertex_PCU( Vec2( mins.x, maxs.y ), color, Vec2( uvAtMins.x,uvAtMaxs.y ) ) );

	DrawVertexArray( 6, &verts[0] );
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawOBB2D( const OBB2& bounds, const Rgba8& color /*= Rgba8::WHITE*/, const Vec2& uvAtMins /*= Vec2::ZERO*/, const Vec2& uvAtMaxs /*= Vec2::ONE */ )
{
	Vec2 fourCorners[4];
	bounds.GetCornerPositions( fourCorners );
	Vertex_PCU bottomLeft = Vertex_PCU( fourCorners[0], color, uvAtMins );
	Vertex_PCU upperRight = Vertex_PCU( fourCorners[2], color, uvAtMaxs );

	Vertex_PCU verts[6];
	verts[0] = (bottomLeft);
	verts[1] = (Vertex_PCU( fourCorners[1], color, Vec2( uvAtMaxs.x, uvAtMins.y ) ));
	verts[2] = (upperRight);
	verts[3] = (bottomLeft);
	verts[4] = (upperRight);
	verts[5] = (Vertex_PCU( fourCorners[3], color, Vec2( uvAtMins.x, uvAtMaxs.y ) ));

	DrawVertexArray( 6, &verts[0] );
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawRing2D( const Vec2& center, float radius, float thickness, const Rgba8& color )
{
	constexpr int vertNum = 6 * CIRCLE_FRAGMENT_NUM;
	Vertex_PCU verts[vertNum];

	Vec2* outterRingVerts = new Vec2[CIRCLE_FRAGMENT_NUM];
	Vec2* innerRingVerts = new Vec2[CIRCLE_FRAGMENT_NUM];
	float unitDegrees = 360.f / (float)CIRCLE_FRAGMENT_NUM;
	float halfThick = thickness * .5f;
	for( int vertIndex = 0; vertIndex < CIRCLE_FRAGMENT_NUM; vertIndex++ )
	{
		float degrees = unitDegrees * (float)vertIndex;
		outterRingVerts[vertIndex] = center + Vec2::MakeFromPolarDegrees( degrees, radius + halfThick );
		innerRingVerts[vertIndex] = center + Vec2::MakeFromPolarDegrees( degrees, radius - halfThick );
	}

	for( int vertIndex = 0; vertIndex < CIRCLE_FRAGMENT_NUM - 1; vertIndex++ )
	{
		int triangleIndex = 6 * vertIndex;
		Vec2& innerBig = innerRingVerts[vertIndex + 1];
		Vec2& outterSmall = outterRingVerts[vertIndex];
		//Init triangle inner small
		verts[triangleIndex] = Vertex_PCU( innerRingVerts[vertIndex], color );
		verts[triangleIndex + 1] = Vertex_PCU( outterSmall, color );
		verts[triangleIndex + 2] = Vertex_PCU( innerBig, color );
		//init triangle outter big
		verts[triangleIndex + 3] = Vertex_PCU( innerBig, color );
		verts[triangleIndex + 4] = Vertex_PCU( outterSmall, color );
		verts[triangleIndex + 5] = Vertex_PCU( outterRingVerts[vertIndex + 1], color );
	}
	//deal with last fragment
	int triangleIndex = 6 * (CIRCLE_FRAGMENT_NUM - 1);
	Vec2& innerBig = innerRingVerts[0];
	Vec2& outterSmall = outterRingVerts[CIRCLE_FRAGMENT_NUM - 1];
	//Init triangle inner small
	verts[triangleIndex] = Vertex_PCU( innerRingVerts[CIRCLE_FRAGMENT_NUM - 1], color );
	verts[triangleIndex + 1] = Vertex_PCU( outterSmall, color );
	verts[triangleIndex + 2] = Vertex_PCU( innerBig, color );
	//init triangle outter big
	verts[triangleIndex + 3] = Vertex_PCU( innerBig, color );
	verts[triangleIndex + 4] = Vertex_PCU( outterSmall, color );
	verts[triangleIndex + 5] = Vertex_PCU( outterRingVerts[0], color );

	DrawVertexArray( vertNum, verts );

	delete[]outterRingVerts;
	delete[] innerRingVerts;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::DrawCapsule2D( const Capsule2& capsule, const Rgba8& color /*= Rgba8::WHITE*/, int fragmentNum /*= CIRCLE_FRAGMENT_NUM */ )
{
	Vec2 startToEnd = capsule.boneEnd - capsule.boneStart;
	Vec2 radiusUp( -startToEnd.y, startToEnd.x );
	radiusUp.SetLength( capsule.radius );
	OBB2 bone( capsule.GetCenter(), Vec2( startToEnd.GetLength(), capsule.radius * 2.f ), startToEnd.GetNormalized() );
	DrawOBB2D(bone,color);

	float unitDegrees = 360.f / (float)CIRCLE_FRAGMENT_NUM;
	Vec2 iLocal = Vec2::MakeFromPolarDegrees( unitDegrees );
	Vec2 jLocal( -iLocal.y, iLocal.x );
	Vec2 lastPoint = radiusUp;
	Vec2 curPoint = radiusUp;
	int halfPointsNum = (int)((float)fragmentNum * .5f);
	Vertex_PCU* circleVerts = new Vertex_PCU[6*halfPointsNum+6];

	//draw start semi circle
	for( int pIdx = 0; pIdx < halfPointsNum; pIdx++ )
	{
		curPoint = curPoint.x * iLocal + curPoint.y * jLocal;
		circleVerts[3*pIdx]= Vertex_PCU( capsule.boneStart, color );
		circleVerts[3*pIdx+1]= Vertex_PCU( capsule.boneStart + lastPoint, color );
		circleVerts[3*pIdx+2]=Vertex_PCU( capsule.boneStart + curPoint, color );
		lastPoint = curPoint;
	}
	int curNum = 3 * halfPointsNum;
	circleVerts[curNum]=Vertex_PCU( capsule.boneStart, color );
	circleVerts[curNum+1]= Vertex_PCU( capsule.boneStart + curPoint, color );
	circleVerts[curNum+2]= Vertex_PCU( capsule.boneStart - radiusUp, color );

	//draw end semi circle
	curNum += 3;
	curPoint = -radiusUp;
	lastPoint = -radiusUp;
	for( int pIdx = 0; pIdx < halfPointsNum; pIdx++ )
	{
		curPoint = curPoint.x * iLocal + curPoint.y * jLocal;
		circleVerts[curNum+3*pIdx]= Vertex_PCU( capsule.boneEnd, color );
		circleVerts[curNum+3*pIdx+1]= Vertex_PCU( capsule.boneEnd + lastPoint, color );
		circleVerts[curNum+3*pIdx+2]= Vertex_PCU( capsule.boneEnd + curPoint, color );
		lastPoint = curPoint;
	}
	curNum = 6 * halfPointsNum + 3;
	circleVerts[curNum]=Vertex_PCU( capsule.boneEnd, color );
	circleVerts[curNum+1]=Vertex_PCU( capsule.boneEnd + curPoint, color );
	circleVerts[curNum+2]=Vertex_PCU( capsule.boneEnd + radiusUp, color );
	DrawVertexArray( 6 * (halfPointsNum + 1), &circleVerts[0] );
	delete[] circleVerts;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::CreateDefaultShaderState()
{
	m_defaultShaderState = new ShaderState(this);
	m_defaultShaderState->m_shader = m_defaultShader;
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::CreateDefaultRasterizeState()
{
    D3D11_RASTERIZER_DESC desc;

    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.FrontCounterClockwise = TRUE; // the only reason we're doing this; 
    desc.DepthBias = 0U;
    desc.DepthBiasClamp = 0.0f;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthClipEnable = TRUE;
    desc.ScissorEnable = FALSE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;

    m_device->CreateRasterizerState(&desc, &m_defaultRasterState);
	m_context->RSSetState(m_defaultRasterState);
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::CreateBlendStates()
{
	//alpha blend state 
    D3D11_BLEND_DESC alphaDesc;
    alphaDesc.AlphaToCoverageEnable = false;
    alphaDesc.IndependentBlendEnable = false;
    alphaDesc.RenderTarget[0].BlendEnable = true;
    alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_device->CreateBlendState(&alphaDesc, &m_alphaBlendStateHandle);
    
	//additive blend state
    D3D11_BLEND_DESC additiveDesc;
    additiveDesc.AlphaToCoverageEnable = false;
    additiveDesc.IndependentBlendEnable = false;
    additiveDesc.RenderTarget[0].BlendEnable = true;
    additiveDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    additiveDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//TODO actual definition should be ONE
    additiveDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    additiveDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    additiveDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    additiveDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    additiveDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_device->CreateBlendState(&alphaDesc, &m_additiveBlendStateHandle);

	//opaque blend state
    D3D11_BLEND_DESC opaqueDesc;
    opaqueDesc.AlphaToCoverageEnable = false;
    opaqueDesc.IndependentBlendEnable = false;
    opaqueDesc.RenderTarget[0].BlendEnable = false;
    opaqueDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    opaqueDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    opaqueDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    opaqueDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    opaqueDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    opaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_device->CreateBlendState(&opaqueDesc, &m_opaqueBlendStateHandle);
}

//////////////////////////////////////////////////////////////////////////
bool RenderContext::CreateTextureFromFile( const char* filePath, bool flipVertical )
{
	Texture* newTexture = Texture::CreateTextureFromFile(this,filePath, flipVertical);
	if (newTexture != nullptr) {
		m_loadedTextures.push_back(newTexture);
		return true;
	}
	else {
		g_theConsole->PrintError(Stringf("load %s failed", filePath));
		return false;
	}
}
