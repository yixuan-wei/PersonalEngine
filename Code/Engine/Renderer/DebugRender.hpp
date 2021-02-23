#pragma once

#include "Engine/Core/Rgba8.hpp"

struct Vec2;
struct Vec3;
struct Vec4;
struct OBB3;
struct AABB2;
struct AABB3;
struct Mat44;
class Camera;
class Texture;
class GPUMesh;
class RenderContext;

enum eDebugRenderMode
{
    DEBUG_RENDER_ALWAYS,
    DEBUG_RENDER_USE_DEPTH,
    DEBUG_RENDER_XRAY,
};

//Set up
void DebugRenderSystemStartup(RenderContext* ctx);
void DebugRenderSystemShutdown();

//control
bool IsDebugRenderingEnabled();
void EnableDebugRendering();
void DisableDebugRendering();
void ClearDebugRendering();

//output
void DebugRenderBeginFrame();               //nothing
void DebugRenderWorldToCamera(Camera* cam);
void DebugRenderScreenTo(Texture* tex);
void DebugRenderEndFrame();

//------------------------------------------------------------------------
// World Rendering
//------------------------------------------------------------------------
//points
void DebugAddWorldPoint(Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldPoint(Vec3 pos, float size, Rgba8 color, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldPoint(Vec3 pos, Rgba8 color, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

//lines
void DebugAddWorldLine(Vec3 p0, Rgba8 startColorP0, Rgba8 endColorP0, Vec3 p1, Rgba8 startColorP1, Rgba8 endColorP1, 
    float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldLine(Vec3 p0, Vec3 p1, Rgba8 color, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

//line strip 
void DebugAddWorldLineStrip(unsigned int count, Vec3 const* points, Rgba8 startColor0, Rgba8 endColor0,
    Rgba8 startColorN, Rgba8 endColorN, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

//arrows
void DebugAddWorldArrow(Vec3 p0, Rgba8 startColorP0, Rgba8 endColorP0, Vec3 p1, Rgba8 startColorP1, Rgba8 endColorP1,
    float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldArrow(Vec3 start, Vec3 end, Rgba8 color, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

//Quads
void DebugAddWorldQuad(Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 startColor, Rgba8 endColor, 
    float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);//TODO remove uv, have new func with texture

//bounds
void DebugAddWorldWireBounds(OBB3 bounds, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldWireBounds(OBB3 bounds, Rgba8 color, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldWireBounds(AABB3 bounds, Rgba8 color, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

void DebugAddWorldWireSphere(Vec3 pos, float radius, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldWireSphere(Vec3 pos, float radius, Rgba8 color, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

//basis
void DebugAddWorldBasis(Mat44 basis, Rgba8 startTint, Rgba8 endTint, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWorldBasis(Mat44 basis, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

//text
//world oriented
void DebugAddWorldText(Mat44 basis, AABB2 bounds, Vec2 alignment, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text);
void DebugAddWorldText(Mat44 basis, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text);
void DebugAddWorldTextf(Mat44 basis, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* format, ...);
void DebugAddWorldTextf(Mat44 basis, Vec2 pivot, Rgba8 color, char const* format, ...);
//camera faced
void DebugAddWorldBillboardText(Vec3 origin, AABB2 bounds, Vec2 alignment, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text);
void DebugAddWorldBillboardText(Vec3 origin, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text);
void DebugAddWorldBillboardTextf(Vec3 origin, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* format, ...);
void DebugAddWorldBillboardTextf(Vec3 origin, Vec2 pivot, Rgba8 color, char const* format, ...);

//grid
void DebugAddWorldGrid(Vec3 origin,
    Vec3 i, float iMin, float iMax, float iMinorSegment, float iMajorSegment, Rgba8 iMinorColor, Rgba8 iMajorColor,
    Vec3 j, float jMin, float jMax, float jMinorSegment, float jMajorSegment, Rgba8 jMinorColor, Rgba8 jMajorColor,
    Rgba8 axisColor);
void DebugAddWorldXYGrid();
void DebugAddWorldXZGrid();
void DebugAddWorldYZGrid();

//mesh
void DebugAddWireMeshToWorld(Mat44 transform, GPUMesh* mesh, Rgba8 startTint, Rgba8 endTint, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
void DebugAddWireMeshToWorld(Mat44 transform, GPUMesh* mesh, Rgba8 tint = Rgba8::WHITE, float duration = 0.0f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

//------------------------------------------------------------------------
// Screen Rendering
//------------------------------------------------------------------------
void DebugRenderSetScreenHeight(float height); // default to 1080.0f when system starts up.  Meaning (0,0) should always be bottom left, (aspect * height, height) is top right
AABB2 DebugGetScreenBounds();                    // useful if you want to align to top right for something

// points
void DebugAddScreenPoint(Vec2 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration);
void DebugAddScreenPoint(Vec2 pos, float size, Rgba8 color, float duration = 0.0f);
void DebugAddScreenPoint(Vec2 pos, Rgba8 color); // assumed size;

// lines
void DebugAddScreenLine(Vec2 p0, Rgba8 startColorP0, Rgba8 endColorP0,
    Vec2 p1, Rgba8 startColorP1, Rgba8 endColorP1,
    float duration);
void DebugAddScreenLine(Vec2 p0, Vec2 p1, Rgba8 color, float duration = 0.0f);

// arrows
void DebugAddScreenArrow(Vec2 p0, Rgba8 startColorP0, Rgba8 endColorP0,
    Vec2 p1, Rgba8 startColorP1, Rgba8 endColorP1,
    float duration);
void DebugAddScreenArrow(Vec2 p0, Vec2 p1, Rgba8 color, float duration = 0.0f);

// quad
void DebugAddScreenQuad(AABB2 bounds, Rgba8 startColor, Rgba8 endColor, float duration);
void DebugAddScreenQuad(AABB2 bounds, Rgba8 color, float duration = 0.0f);

// texture
void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 startTint, Rgba8 endTint, float duration = 0.0f);
void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 tint, float duration = 0.0f);
void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, Rgba8 tint = Rgba8::WHITE, float duration = 0.0f); // assume UVs are full texture

// text
void DebugAddScreenText(AABB2 bounds, Vec2 alignment, float size, Rgba8 startColor, Rgba8 endColor, float duration, char const* text);
void DebugAddScreenText( Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, char const* text);
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, char const* format, ...);
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 color, float duration, char const* format, ...);
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 color, char const* format, ...);
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, Rgba8 color, char const* format, ...);

// screen basis [extra]
void DebugAddScreenBasis(Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 startTint, Rgba8 endTint, float duration);
void DebugAddScreenBasis(Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 tint = Rgba8::WHITE, float duration = 0.0f);

// message log system [extra]
void DebugAddMessage(float duration, Rgba8 color, char const* format, ...);
void DebugAddMessage(Rgba8 color, char const* format, ...);

