#pragma once

#include "Engine/Renderer/ShaderStage.hpp"

struct ID3D11RasterizerState;
struct ID3D11InputLayout;
struct buffer_attribute_t;
class RenderContext;

class Shader
{
public:
	static Shader* CreateDefaultShader(RenderContext* context);
	static Shader* CreateErrorShader(RenderContext* context);

	Shader( RenderContext* context );
	~Shader();

	bool CreateFromFile( std::string const& filename );

	 // for hooking IA (input assembler) to the VS (vertex shader), 
	  // needs to vertex shader and vertex format to make the binding
	ID3D11InputLayout* GetOrCreateInputLayout(buffer_attribute_t const* layout);            // A02

public:
	RenderContext* m_owner = nullptr;

	ShaderStage m_vertexStage; //value for owning.
	ShaderStage m_fragmentStage;

	 // for now, we'll have 1, but in the future you could have one for each different vertex type you use with this; 
	ID3D11InputLayout* m_inputLayout = nullptr;
	buffer_attribute_t const* m_lastLayout = nullptr;
};