#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/buffer_attribute_t.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
Shader* Shader::CreateDefaultShader(RenderContext* context)
{
	std::string defaultSrc = R"(
struct vs_input_t
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct light_t
{
	float3 worldPosition; 
    //padding 4 bytes after
	float3 color;
	float intensity;
    float3 attenuation;
    //padding 4 bytes after
    float3 specAttenuation;
};

cbuffer time_constants : register(b0)
{
    float SYSTEM_TIME_SECONDS;
    float SYSTEM_TIME_DELTA_SECONDS;

    float GAMMA;
};

cbuffer camera_constants : register(b1)
{
    float4x4 PROJECTION; 
    float4x4 VIEW;
    float3 CAM_POSITION;
    //padding 4 bytes after
};

cbuffer object_constants : register(b2)
{
	float4x4 MODEL;
	float4 TINT;
};

cbuffer light_constants : register(b3)
{
	float4 AMBIENT;
	light_t LIGHT;
};

Texture2D<float4> tDiffuse : register(t0);
Texture2D<float4> tNormal : register(t1);
Texture2D<float4> tSpecular : register(t2);
SamplerState sSampler : register(s0);

struct v2f_t
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

v2f_t VertexFunction(vs_input_t input)
{
    v2f_t v2f = (v2f_t)0;

    v2f.color = input.color;
    v2f.uv = input.uv;

    float4 modelPos = float4(input.position, 1.0f);
	float4 worldPos = mul(MODEL, modelPos);
    float4 cameraPos = mul(VIEW, worldPos);
    float4 clipPos = mul(PROJECTION, cameraPos);
    v2f.position = clipPos;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{
    float4 color = tDiffuse.Sample(sSampler,input.uv);
    float4 finalColor = color * input.color;
	finalColor = finalColor * TINT;
    clip(finalColor.a-0.0001f);
	return finalColor;
}
)";

	Shader* result = new Shader(context);

	result->m_vertexStage.Compile(context, "Default", &defaultSrc[0], defaultSrc.size(), eShaderType::SHADER_TYPE_VERTEX);
	result->m_fragmentStage.Compile(context, "Default", &defaultSrc[0], defaultSrc.size(), eShaderType::SHADER_TYPE_FRAGMENT);

	GUARANTEE_OR_DIE(result->m_vertexStage.IsValid() && result->m_fragmentStage.IsValid(), "Default Shader Construct failed");

	return result;
}

//////////////////////////////////////////////////////////////////////////
Shader* Shader::CreateErrorShader(RenderContext* context)
{
    std::string errorSrc = R"(
struct vs_input_t
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer time_constants : register(b0)
{
    float SYSTEM_TIME_SECONDS;
    float SYSTEM_TIME_DELTA_SECONDS;
};

cbuffer camera_constants : register(b1)
{
    float4x4 PROJECTION; 
    float4x4 VIEW;
};

cbuffer object_constants : register(b2)
{
	float4x4 MODEL;
	float4 TINT;
};

Texture2D<float4> tDiffuse : register(t0);
SamplerState sSampler : register(s0);

struct v2f_t
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

v2f_t VertexFunction(vs_input_t input)
{
    v2f_t v2f = (v2f_t)0;

    v2f.color = input.color;
    v2f.uv = input.uv;

    float4 modelPos = float4(input.position, 1.0f);
	float4 worldPos = mul(MODEL, modelPos);
    float4 cameraPos = mul(VIEW, worldPos);
    float4 clipPos = mul(PROJECTION, cameraPos);
    v2f.position = clipPos;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{
    float4 color = float4(0.0f,0.0f,0.0f,1.0f);
	float step = 0.05f;
	float rangeX = sin(step*(input.position.x+1.0f));
	float rangeY = sin(step*(input.position.y+1.0f));
	color.r=color.b=ceil(rangeX*rangeY);
	
    return color;
}
)";

    Shader* result = new Shader(context);

    result->m_vertexStage.Compile(context, "Error", &errorSrc[0], errorSrc.size(), eShaderType::SHADER_TYPE_VERTEX);
    result->m_fragmentStage.Compile(context, "Error", &errorSrc[0], errorSrc.size(), eShaderType::SHADER_TYPE_FRAGMENT);

    GUARANTEE_OR_DIE(result->m_vertexStage.IsValid() && result->m_fragmentStage.IsValid(), "Error Shader Construct failed");

    return result;
}

//////////////////////////////////////////////////////////////////////////
Shader::Shader( RenderContext* context )
	:m_owner(context)
{
}

//////////////////////////////////////////////////////////////////////////
Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
}

//////////////////////////////////////////////////////////////////////////
bool Shader::CreateFromFile( std::string const& filename )
{
	size_t fileSize = 0;
	void* src = FileReadToNewBuffer( filename, &fileSize );
	if( src == nullptr )
	{
		return false;
	}

	m_vertexStage.Compile(m_owner,filename,src,fileSize,eShaderType::SHADER_TYPE_VERTEX);
	m_fragmentStage.Compile(m_owner,filename,src,fileSize,eShaderType::SHADER_TYPE_FRAGMENT);

	delete[] src;
	return m_vertexStage.IsValid() && m_fragmentStage.IsValid();
}

//////////////////////////////////////////////////////////////////////////
ID3D11InputLayout* Shader::GetOrCreateInputLayout(buffer_attribute_t const* layout)
{
	if (m_inputLayout != nullptr && m_lastLayout==layout)
	{
		return m_inputLayout;
	}
	
	DX_SAFE_RELEASE(m_inputLayout);
	D3D11_INPUT_ELEMENT_DESC vertexDesc[VERTEX_DESCRIPTION_NUM];
	int idx = 0;
	while (layout[idx].name != "")
	{
        vertexDesc[idx].SemanticIndex        = 0; //index in array in shader
        vertexDesc[idx].SemanticName         = layout[idx].name.c_str();//name in shader
        vertexDesc[idx].AlignedByteOffset    = layout[idx].offset;
        vertexDesc[idx].Format               = ToDXGIFormat(layout[idx].type);
        vertexDesc[idx].InputSlot            = 0;//interlaced format just got one slot
        vertexDesc[idx].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA; //Instance Data is for crowd
        vertexDesc[idx].InstanceDataStepRate = 0;//not Using instance data

		idx++;
	}

	m_lastLayout = layout;
	ID3D11Device* device = m_owner->m_device;
	device->CreateInputLayout(
		vertexDesc, idx, //describe vertex
		m_vertexStage.GetByteCode(), m_vertexStage.GetByteCodeLength(),
		&m_inputLayout );

	return m_inputLayout;
}
