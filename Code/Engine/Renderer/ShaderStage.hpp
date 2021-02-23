#pragma once

#include <string>

class RenderContext;
struct ID3D10Blob;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

enum eShaderType
{
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_FRAGMENT
};

class ShaderStage
{
public:
    ~ShaderStage();

    bool Compile(RenderContext* ctx, std::string const& filename,
        void const* source, size_t const sourceByteLen, eShaderType stage);

    inline bool IsValid() const { return (m_handle != nullptr); }

    void const* GetByteCode() const;
    size_t GetByteCodeLength() const;

public:
    eShaderType m_type;
    ID3D10Blob* m_byteCode = nullptr;
    union
    {
        ID3D11Resource* m_handle;
        ID3D11VertexShader* m_vs;
        ID3D11PixelShader* m_fs;
    };
};