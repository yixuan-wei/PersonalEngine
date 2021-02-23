#pragma once

#include "Engine/Core/Rgba8.hpp"
#include <vector>

class ShaderState;
class Texture;
class Sampler;
class RenderBuffer;
class RenderContext;

class Material
{
public:
    ~Material();

    void SetFromFile(char const* filePath, RenderContext* context);

    void UpdateUBO(RenderContext* context);

    void SetData(void const* data, size_t dataSize);

    template <typename UBO_STRUCT_TYPE>
    void SetData(UBO_STRUCT_TYPE const& data)
    {
        SetData(&data, sizeof(UBO_STRUCT_TYPE));
    }

    template <typename UBO_STRUCT_TYPE>
    UBO_STRUCT_TYPE* GetDataAs()
    {
        m_uboDirty = true;
        if (m_uboCPUData.size() == sizeof(UBO_STRUCT_TYPE)) {
            return (UBO_STRUCT_TYPE*)&m_uboCPUData[0];
        }
        else {
            m_uboCPUData.resize(sizeof(UBO_STRUCT_TYPE));
            return (UBO_STRUCT_TYPE*)&m_uboCPUData[0];
        }
    }

public:
    ShaderState* m_shaderState = nullptr;

    Rgba8 m_tint = Rgba8::WHITE;
    float m_specularFactor = 1.f;
    float m_specularPower = 1.f;

    Texture* m_diffuseTexture = nullptr;
    Texture* m_normalTexture = nullptr;
    Texture* m_specularTexture = nullptr;
    Texture* m_emissiveTexture = nullptr;

    std::vector<Texture*> m_userTextures;
    std::vector<Sampler*> m_userSamplers;

    std::vector<unsigned char> m_uboCPUData;
    bool m_uboDirty = true;
    RenderBuffer* m_ubo = nullptr;
};