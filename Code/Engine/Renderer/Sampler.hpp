#pragma once

struct ID3D11SamplerState;
class RenderContext;

enum eSamplerType
{
    SAMPLER_POINT,      //pixelated look, nearest
    SAMPLER_BILINEAR,   //smoother, no mips
    SAMPLER_LINEAR_BORDER
};

class Sampler 
{
public:
    Sampler(RenderContext* ctx, eSamplerType type);
    ~Sampler();
    
    inline ID3D11SamplerState* GetHandle() const { return m_handle; }

public:
    RenderContext* m_owner = nullptr;
    ID3D11SamplerState* m_handle = nullptr;
};