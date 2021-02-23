#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
Material::~Material()
{
    delete m_ubo;

    for (Sampler* samp : m_userSamplers) {
        if (samp != nullptr) {
            delete samp;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Material::SetFromFile(char const* filePath, RenderContext* context)
{
    XmlDocument matFile;
    XmlError code = matFile.LoadFile(filePath);
    GUARANTEE_OR_DIE(code == XmlError::XML_SUCCESS, Stringf("Error when loading %s", filePath));

    XmlElement* root = matFile.RootElement();
    std::string shaderStatePath = ParseXmlAttribute(*root, "shaderState", "data/shaders/states/lit.shaderstate");
    m_shaderState = context->CreateOrGetShaderState(shaderStatePath.c_str());
    m_tint = ParseXmlAttribute(*root, "tint", Rgba8::WHITE);
    m_specularFactor = ParseXmlAttribute(*root, "specularFactor", 1.f);
    m_specularPower = ParseXmlAttribute(*root, "specularPower", 32.f);

    std::string diffusePath = ParseXmlAttribute(*root, "diffuse", "White");
    std::string normalPath = ParseXmlAttribute(*root, "normal", "Flat");
    std::string specPath = ParseXmlAttribute(*root, "spec", "White");
    std::string emsvPath = ParseXmlAttribute(*root, "emissive", "Black");
    m_diffuseTexture = context->CreateOrGetTextureFromFile(diffusePath.c_str());
    m_normalTexture = context->CreateOrGetTextureFromFile(normalPath.c_str());
    m_specularTexture = context->CreateOrGetTextureFromFile(specPath.c_str());
    m_emissiveTexture = context->CreateOrGetTextureFromFile(emsvPath.c_str());

    XmlElement* textures = root->FirstChildElement("Textures");
    if (textures != nullptr) {
        XmlElement* tex = textures->FirstChildElement("Texture");
        while (tex != nullptr) {
            std::string texPath = ParseXmlAttribute(*tex, "file", "White");
            Texture* newTex = context->CreateOrGetTextureFromFile(texPath.c_str());
            m_userTextures.push_back(newTex);
            tex = tex->NextSiblingElement("Texture");
        }
    }

    XmlElement* samplers = root->FirstChildElement("Samplers");
    if (samplers != nullptr) {
        XmlElement* samp = samplers->FirstChildElement("Sampler");
        while (samp != nullptr) {
            eSamplerType type = ParseXmlAttribute(*samp, "type", SAMPLER_POINT);
            Sampler* sampler = new Sampler(context, type);
            m_userSamplers.push_back(sampler);
            samp = samp->NextSiblingElement("Sampler");
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Material::UpdateUBO(RenderContext* context)
{
    if (m_ubo == nullptr) {
        m_ubo = new RenderBuffer(context, eRenderBufferUsageBit::UNIFORM_BUFFER_BIT, eRenderMemoryHint::MEMORY_HINT_DYNAMIC);
    }

    if (!m_uboDirty || m_uboCPUData.size()<1) {
        return;
    }
    
    m_ubo->Update(&m_uboCPUData[0], m_uboCPUData.size(), m_uboCPUData.size());
    m_uboDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void Material::SetData(void const* data, size_t dataSize)
{
    m_uboCPUData.resize(dataSize);
    memcpy(&m_uboCPUData[0], data, dataSize);
    m_uboDirty = true;
}
