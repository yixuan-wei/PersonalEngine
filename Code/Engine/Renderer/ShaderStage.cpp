#include "Engine/Renderer/ShaderStage.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////
static char const* GetDefaultEntryPointForStage(eShaderType type)
{
    switch (type)
    {
    case eShaderType::SHADER_TYPE_VERTEX:	return "VertexFunction";
    case eShaderType::SHADER_TYPE_FRAGMENT:	return "FragmentFunction";
    default:                                ERROR_AND_DIE("Bad shader Stage");
    }
}

//////////////////////////////////////////////////////////////////////////
static char const* GetShaderModelForStage(eShaderType type)
{
    switch (type)
    {
    case SHADER_TYPE_VERTEX:   return "vs_5_0";
    case SHADER_TYPE_FRAGMENT: return "ps_5_0";
    default:                   ERROR_AND_DIE("Bad shader stage");
    }
}

//////////////////////////////////////////////////////////////////////////
ShaderStage::~ShaderStage()
{
    DX_SAFE_RELEASE(m_byteCode);
    DX_SAFE_RELEASE(m_handle);
}

//////////////////////////////////////////////////////////////////////////
bool ShaderStage::Compile(RenderContext* ctx, std::string const& filename, void const* source, size_t const sourceByteLen, eShaderType stage)
{
    //HLSL-high level shading languages
    //compile HLSL to byteCode
    //link ByteCode -> device assembly

    char const* entrypoint = GetDefaultEntryPointForStage(stage);
    char const* shaderModel = GetShaderModelForStage(stage);

    DWORD compileFlags = 0U;
#if defined(DEBUG_SHADERS)
    compile_flags |= D3DCOMPILE_DEBUG;
    compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
    compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
   // compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
    compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

    ID3DBlob* byteCode = nullptr; //Binary Large Objects
    ID3DBlob* errors = nullptr;

    HRESULT hr = ::D3DCompile(source,    //plain text HLSL code
        sourceByteLen,                        //byte length of text source
        filename.c_str(),                     //optional, used for error messages
        nullptr,                              //pre-compiler defines, 
        D3D_COMPILE_STANDARD_FILE_INCLUDE,    //include rules, allow #include in shader
        entrypoint,                           //shader entry point
        shaderModel,                          //compile target (specifying compiler targets)
        compileFlags,                         //flags for compilation
        0,                                    //effect flags
        &byteCode,                            //[OUT] ID3DBlob (buffer) storing byte code
        &errors);                            //[OUT] ID3DBlob (buffer) storing error info

    if (FAILED(hr))
    {
        if (errors != nullptr)
        {
            char* error_string = (char*)errors->GetBufferPointer();
            DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s", filename.c_str(), error_string);
            g_theConsole->PrintError(Stringf("Failed to compile [%s].  Compiler gave the following output;\n%s",
                filename.c_str(), error_string));
            return false;
        }
    }
    else
    {
        ID3D11Device* device = ctx->m_device;
        void const* byteCodePtr = byteCode->GetBufferPointer();
        size_t byteCodeSize = byteCode->GetBufferSize();
        switch (stage)
        {
        case eShaderType::SHADER_TYPE_VERTEX:
        {
            hr = device->CreateVertexShader(byteCodePtr, byteCodeSize, nullptr, &m_vs);
            GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to link shader stage");
            break;
        }
        case eShaderType::SHADER_TYPE_FRAGMENT:
        {
            hr = device->CreatePixelShader(byteCodePtr, byteCodeSize, nullptr, &m_fs);
            GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to link shader stage");
            break;
        }
        default: ERROR_AND_DIE("Unimplemented stage"); break;
        }
    }

    DX_SAFE_RELEASE(errors);

    if (stage == eShaderType::SHADER_TYPE_VERTEX)
    {
        m_byteCode = byteCode;
    }
    else
    {
        DX_SAFE_RELEASE(byteCode);
        m_byteCode = nullptr;
    }

    m_type = stage;

    return IsValid();
}

//////////////////////////////////////////////////////////////////////////
void const* ShaderStage::GetByteCode() const
{
    return m_byteCode->GetBufferPointer();
}

//////////////////////////////////////////////////////////////////////////
size_t ShaderStage::GetByteCodeLength() const
{
    return m_byteCode->GetBufferSize();
}
