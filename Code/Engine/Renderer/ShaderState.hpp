#pragma once

#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Core/XMLUtils.hpp"

class Shader;
class RenderContext;

class ShaderState
{
    friend class RenderContext;

public:
    ShaderState(RenderContext* context);

    void SetupFromXML(XmlElement const& node);

public:
    RenderContext* m_context = nullptr;

    Shader* m_shader = nullptr;

    eBlendMode m_blendMode = BLEND_ALPHA;
    eCompareFunc m_depthCompare = COMPARE_FUNC_LEQUAL;
    bool m_writeDepth = true;
    bool m_windOrderCounterClockwise = true;
    eCullMode m_cullMode = CULL_BACK;
    eFillMode m_fillMode = FILL_SOLID;
};