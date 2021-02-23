#pragma once

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include <vector>
#include <map>
#include <string>

class Clock;
class Texture;
class BitmapFont;
class Window;
class SwapChain;
class Shader;
class ShaderState;
class Sampler;
class VertexBuffer;
class RenderBuffer;
class IndexBuffer;
class GPUMesh;
class Material;
struct IntVec2;
struct OBB2;
struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;

//////////////////////////////////////////////////////////////////////////
class RenderContext
{
public:
	RenderContext() = default;
	~RenderContext() = default;

	void Startup( Window* window);
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void SetupParentClock(Clock* gameClock);

	void SetGamma(float newGamma);
	float GetGamma() const { return m_gamma; }

	void BeginCamera( Camera const* camera );
	void EndCamera( Camera const* camera );	

	void BindIndexBuffer(IndexBuffer* ibo);
	void BindVertexBuffer( VertexBuffer* vbo );
	void BindUniformBuffer(unsigned int slot, RenderBuffer* ubo);//UBO-uniform buffer object

	void ReloadShaders();
	void BindMaterial(Material* mat);
	void BindShaderState(ShaderState const* shaderState);
	void BindShaderStateByName(char const* filename);
	void BindSampler(Sampler const* sampler);
	void BindUserSampler(unsigned int slot, Sampler const* sampler);

    void BindDiffuseTexture(Texture const* texture);
	void BindNormalTexture(Texture const* texture);
	void BindSpecularTexture(Texture const* texture);
	void BindEmissiveTexture(Texture const* texture);
	void BindUserTexture(unsigned int idx, Texture const* texture);
	void BindDiffuseTexture(char const* filename);
	void BindNormalTexture(char const* filename);
	void BindSpecularTexture(char const* filename);
	void BindEmissiveTexture(char const* filename);
	void BindUserTexture(unsigned int idx, char const* filename);

    void ClearRenderTargetView(const Rgba8& clearColor);
    void ClearDepth(Texture* depthStencilTexture, float depth);
	void EnableDepth(eCompareFunc func, bool writeDepthOnPass);
    void DisableDepth();
	void SetCullMode(eCullMode cullMode);
	void SetFillMode(eFillMode fillMode);
	void SetFrontFaceWindOrder(bool isCounterClockwise);

    void SetBlendMode(eBlendMode blendmode);
	void SetModelMatrix(Mat44 const& newModelMatrix);
	void SetModelTint(Rgba8 const& newTint);
	void SetModelData(Mat44 const& newModelMatrix, Rgba8 const& newTint);
	void SetMaterialBuffer(unsigned int idx, RenderBuffer* buffer);

	void EnableFog(float nearFog, float farFog, Rgba8 const& fogColor);
	void DisableFog();

	void EnableLight(unsigned int idx, light_t const& lightInfo);
	void DisableLight(unsigned int idx);
	void SetDiffuseFactor(float diffuseFactor = 1.f);
	void SetEmissiveFactor(float emissiveFactor = 1.f);
	void SetSpecularFactors(float specularFactor = 1.f, float specularPower = 32.f);
	void SetAmbientColor(Rgba8 const& color);
	void SetAmbientIntensity(float intensity);
	void SetAmbientLight(Rgba8 const& color, float intensity);
	float GetAmbientIntensity() const;

	Texture* GetFrameColorTarget() const;
	Texture* AcquireRenderTargetMatching(Texture* tex);
	void ReleaseRenderTarget(Texture* tex);	//User call release

	int GetTotalRenderTargetCount() const;
	int GetFreeRenderTargetCount() const;

	void ApplyEffect(Texture* dst, Texture* src, Material* mat);
	void CopyTexture(Texture* dst, Texture* src);


	ShaderState* CreateOrGetShaderState(char const* filename);
	Shader* CreateOrGetShader( char const* filename );
	Texture* CreateOrGetTextureFromFile( const char* filePath, bool reload=false, bool flipVertical=true);
	BitmapFont* CreateOrGetBitmapFont( const char* filePathNoExtension );

	void Draw( int numVertexes, int vertexOffset = 0 );
	void DrawIndexed(int indexCount, int indexOffset = 0, int vertexOffset = 0);
	void DrawMesh(GPUMesh* mesh);
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexArray );

	//draw stuff
	void DrawLine2D( const Vec2& startPoint, const Vec2& endPoint, float thickness, const Rgba8& color );
	void DrawDisc2D( const Vec2& position, float radius, const Rgba8& color, int fragmentNum = CIRCLE_FRAGMENT_NUM );	
	void DrawSquare2D(Vec3 const& position, float width, Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvAtMins = Vec2::ZERO, Vec2 const& uvAtMaxs = Vec2::ONE);
	void DrawAABB2D( const AABB2& bounds, const Rgba8& color = Rgba8::WHITE, const Vec2& uvAtMins = Vec2::ZERO, const Vec2& uvAtMaxs = Vec2::ONE );
	void DrawOBB2D( const OBB2& bounds, const Rgba8& color = Rgba8::WHITE, const Vec2& uvAtMins = Vec2::ZERO, const Vec2& uvAtMaxs = Vec2::ONE );
	void DrawRing2D( const Vec2& center, float radius, float thickness, const Rgba8& color );
	void DrawCapsule2D( const Capsule2& capsule, const Rgba8& color = Rgba8::WHITE, int fragmentNum = CIRCLE_FRAGMENT_NUM );

private:
	void CreateDefaultColorTextures();
	void CreateDefaultShaderState();
	void CreateDefaultRasterizeState();
	void CreateBlendStates();
	bool CreateTextureFromFile( const char* filePath, bool flipVertical=true );
	BitmapFont* CreateBitmapFontFromFile( const char* filePath );
	ShaderState* CreateShaderStateFromFile(char const* filePath);
	Texture* CreateDefaultNormalTexture();	
    Texture* CreateTextureFromColor(Rgba8 color);
    Texture* CreateRenderTarget(IntVec2 const& texelSize);

	void ClearRenderTargetView(ID3D11RenderTargetView* colorTex, float const* color);

	void BindShader(Shader* shader);

	void UpdateInputLayout();
    void UpdateFrameData();
	void UpdateLightData();

private:
	std::vector<Texture*> m_renderTargetPool;
	std::vector<Texture*> m_loadedTextures;
	std::vector< BitmapFont* > m_loadedFonts;
	std::map<std::string,Shader*> m_loadedShaders;
	std::map<std::string, ShaderState*> m_loadedShaderStates;

	Clock* m_gameClock = nullptr;

    SwapChain* m_swapchain = nullptr;
	Texture* m_defaultDepthBuffer = nullptr;

	int m_totalRenderTargetMade = 0;
	Camera* m_effectCamera = nullptr;

	Sampler* m_pointSampler = nullptr;
	Texture* m_texWhite = nullptr;
	Texture* m_texBlack = nullptr;
	Texture* m_texNormal = nullptr;

	float   m_gamma = 2.2f;
	Camera* m_currentCam = nullptr;

	ShaderState* m_defaultShaderState = nullptr;
	bool    m_shaderHasChanged = false;
    Shader* m_currentShader = nullptr;
    Shader* m_defaultShader = nullptr;
	Shader* m_errorShader = nullptr;

	GPUMesh* m_immediateMesh = nullptr;
	RenderBuffer* m_modelUBO = nullptr;
	RenderBuffer* m_frameUBO = nullptr;
	RenderBuffer* m_lightUBO = nullptr;

	object_data_t m_objectData;
	light_data_t m_lightData;
	bool m_lightDirty = false;

	buffer_attribute_t const* m_lastLayout = nullptr;
	buffer_attribute_t const* m_currentLayout = nullptr;

	ID3D11RasterizerState* m_defaultRasterState = nullptr;
	ID3D11RasterizerState* m_tempRasterState = nullptr;

    ID3D11Buffer* m_lastVBH = nullptr;
	ID3D11DepthStencilState* m_currentDepthStencilState = nullptr;

    ID3D11BlendState* m_alphaBlendStateHandle = nullptr;
    ID3D11BlendState* m_additiveBlendStateHandle = nullptr;
	ID3D11BlendState* m_opaqueBlendStateHandle = nullptr;

public:
	Sampler* m_linearSampler = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr; //immediate context
};
