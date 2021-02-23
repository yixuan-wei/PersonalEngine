#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//////////////////////////////////////////////////////////////////////////
ShaderState::ShaderState(RenderContext* context)
    : m_context(context)
{
}

//////////////////////////////////////////////////////////////////////////
void ShaderState::SetupFromXML(XmlElement const& node)
{
    const XmlAttribute* attribute = node.FindAttribute("shader");
    GUARANTEE_OR_DIE(attribute != nullptr, "shader state has not attribute shader");
    m_shader = m_context->CreateOrGetShader(attribute->Value());

    m_windOrderCounterClockwise = ParseXmlAttribute(node, "frontCCW", true);
    m_cullMode = ParseXmlAttribute(node, "cull", CULL_BACK);
    m_fillMode = ParseXmlAttribute(node, "fill", FILL_SOLID);
    m_depthCompare = ParseXmlAttribute(node, "depthTest", COMPARE_FUNC_LEQUAL);
    m_writeDepth = ParseXmlAttribute(node, "depthWrite", true);
    m_blendMode = ParseXmlAttribute(node, "blendMode", BLEND_ALPHA);
}
