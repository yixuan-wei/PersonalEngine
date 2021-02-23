#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/EngineCommon.hpp"

enum eBlendMode
{
    BLEND_ALPHA,
    BLEND_ADDITIVE,
    BLEND_OPAQUE,
};

enum eCompareFunc
{
    COMPARE_FUNC_NEVER,
    COMPARE_FUNC_ALWAYS,
    COMPARE_FUNC_EQUAL,
    COMPARE_FUNC_LEQUAL,
    COMPARE_FUNC_GEQUAL,
    COMPARE_FUNC_GREATER
};

enum eCullMode
{
    CULL_NONE = 1,
    CULL_FRONT,
    CULL_BACK
};

enum eFillMode
{
    FILL_SOLID,
    FILL_WIREFRAME
};

struct object_data_t
{
    Mat44 model;
    Vec4 tint;
};

struct frame_data_t
{
    float system_time;
    float system_delta_time;

    float gamma = 2.2f;
    float padding00;
};

struct light_t
{
    //default to point light
    Vec3 position;
    float padding00;
    Vec3 color = Vec3(1.f, 1.f, 1.f);
    float intensity = 0.f;
    Vec4 direction = Vec4(0.f, 0.f, -1.f, 0.f);
    Vec3 attenuation = Vec3(0.f, 1.f, 0.f);
    float cosInnerAngles = -1.1f;
    Vec3 specAttenuation = Vec3(0.f, 1.f, 0.f);
    float cosOuterAngles = -1.1f;
};

struct light_data_t
{
    Vec4 ambient;
    light_t light[MAX_LIGHT_COUNT];

    float specularFactor = 1.f;
    float specularPower = 32.f;
    float diffuseFactor = 1.f;
    float emissiveFactor = 1.f;

    Vec3 fogColor;
    float fogNear = 3.402823466e+38F;
    float fogFar = 3.402823466e+38F;
    Vec3 padding00;
};

enum eBufferSlot
{
    UBO_FRAME_SLOT = 0,
    UBO_CAMERA_SLOT = 1,
    UBO_OBJECT_SLOT = 2,
    UBO_LIGHT_SLOT = 3,
    UBO_MATERIAL_SLOT = 4,
};

enum eTextureSlot
{
    TEX_DIFFUSE_SLOT = 0,
    TEX_NORMAL_SLOT = 1,
    TEX_SPEC_SLOT = 2,
    TEX_EMISSIVE_SLOT = 3,
    TEX_USER_START_SLOT = 8
};