#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/MeshUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <stdarg.h>

class DebugRenderObject;

static Rgba8 sXrayTint = Rgba8(0, 0, 0, 128);

enum eDebugRenderObjectType
{
    DROBJECT_NORMAL,
    DROBJECT_WIRE_BOUNDS,
    DROBJECT_MESH,
    DROBJECT_TEXT
};

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
class DebugRenderSystem
{
public:
    DebugRenderSystem(RenderContext* ctx);
    ~DebugRenderSystem();

    void AppendNewDebugRenderObject(DebugRenderObject* newObject);
    void CleanDeadObjects();
    void ClearRenderObjects();

    void SetDebugRenderEnabled(bool isEnabled);
    void SetScreenHeight(float newHeight);
    void SetWorldAxisConvention(AxisConvention newConvention);

    std::vector<DebugRenderObject*> GetObjects() const { return m_objects; }
    RenderContext* GetRenderContext() const { return m_context; }
    Camera* GetCamera() const { return m_camera; }
    Clock* GetClock() const { return m_clock; }
    BitmapFont* GetFont() const { return m_font; }
    Mat44 GetWorldAxisMatrix() const;
    float GetScreenHeight() const { return m_screenHeight; }
    float GetDefaultLineWidth() const { return m_defaultLineWidth; }
    bool IsEnabled() const { return m_enabled; }

private:
    std::vector<DebugRenderObject*> m_objects;

    RenderContext* m_context = nullptr;
    Camera* m_camera = nullptr;
    Clock* m_clock = nullptr;
    BitmapFont* m_font = nullptr;

    float m_screenHeight = 1080.f;
    float m_defaultLineWidth = 5.4f;
    bool m_enabled = false;
    AxisConvention m_worldAxisConvention = AXIS_XYZ;
};

//////////////////////////////////////////////////////////////////////////
class DebugRenderObject
{
public:
    DebugRenderObject(double duration, eDebugRenderMode mode, bool isWorldSpace);
    virtual ~DebugRenderObject() = default;

    virtual void AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices) = 0;
    virtual const Texture* GetTexture() const { return nullptr; }

    eDebugRenderMode GetMode() const { return m_mode; }
    eDebugRenderObjectType GetType() const { return m_type; }
    bool IsReadyToBeCulled() const;     // should delete
    bool IsWorldSpace() const { return m_isWorldSpace; }
    bool IsTextured() const { return m_isTextured; }

protected:
    std::vector<Vertex_PCU> m_verts;
    std::vector<unsigned int> m_indices;
    Timer* m_timer = nullptr;
    bool m_isWorldSpace = true;
    bool m_isTextured = false;
    double m_duration = 0.0;
    eDebugRenderObjectType m_type = DROBJECT_NORMAL;
    eDebugRenderMode m_mode = DEBUG_RENDER_USE_DEPTH;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectSimpleOneColor : public DebugRenderObject
{
public:
    DRObjectSimpleOneColor(Rgba8 const& startColor, Rgba8 const& endColor, double duration, eDebugRenderMode mode, bool isWorldSpace);

    virtual void AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices) override;

protected:
    Rgba8  m_startColor;
    Rgba8  m_endColor;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectPoint : public DRObjectSimpleOneColor
{
public:
    DRObjectPoint(Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, double duration, eDebugRenderMode mode, bool isWorldSpace);

private:
    Vec3 m_position;
    float m_size = 0.f;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectLine : public DebugRenderObject
{
public:
    DRObjectLine(Vec3 p0, Rgba8 const& startColor0, Rgba8 const& endColor0,
        Vec3 p1, Rgba8 const& startColor1, Rgba8 const& endColor1, double duration, eDebugRenderMode mode, bool isWorldSpace);

    virtual void AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices) override;

private:
    Vec3 m_start;
    Vec3 m_end;
    Rgba8 m_startColor0;
    Rgba8 m_startColor1;
    Rgba8 m_endColor0;
    Rgba8 m_endColor1;
};

static constexpr int sLineCuts = 4;

//////////////////////////////////////////////////////////////////////////
class DRObjectArrow :public DebugRenderObject
{
public:
    DRObjectArrow(Vec3 p0, Rgba8 const& startColor0, Rgba8 const& endColor0,
        Vec3 p1, Rgba8 const& startColor1, Rgba8 const& endColor1, double duration, eDebugRenderMode mode, bool isWorldSpace);

    virtual void AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices) override;

private:
    Vec3 m_start;
    Vec3 m_end;
    Rgba8 m_startColor0;
    Rgba8 m_startColor1;
    Rgba8 m_endColor0;
    Rgba8 m_endColor1;
};

static constexpr int sArrowCuts = 8;

//////////////////////////////////////////////////////////////////////////
class DRObjectBasis :public DebugRenderObject
{
public:
    DRObjectBasis(Mat44 const& basis, Rgba8 const& startTint, Rgba8 const& endTint, double duration, eDebugRenderMode mode, bool isWorldSpace);

    virtual void AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices) override;

private:
    Mat44 m_basis;
    Rgba8 m_startTint;
    Rgba8 m_endTint;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectScreenBasis : public DebugRenderObject
{
public:
    DRObjectScreenBasis(Vec2 const& origin, Mat44 const& basis, Rgba8 const& startTint, Rgba8 const& endTint,
        float duration);

    virtual void AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices) override;

private:
    Vec2 m_screenOrigin;
    Rgba8 m_startTint;
    Rgba8 m_endTint;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectQuad : public DRObjectSimpleOneColor
{
public:
    DRObjectQuad(Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Vec3 const& p3, 
        Rgba8 const& startColor, Rgba8 const& endColor, double duration, eDebugRenderMode mode, bool isWorldSpace);

private:
    Vec3 m_p0, m_p1, m_p2, m_p3;
    AABB2 m_uvs;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectTexturedQuad : public DRObjectSimpleOneColor
{
public:
    DRObjectTexturedQuad(AABB2 const& bounds, Texture* tex, AABB2 const& uvs,
        Rgba8 const& startTint, Rgba8 const& endTint, double duration, eDebugRenderMode mode);
    
    virtual const Texture* GetTexture() const override { return m_texture; }

private:
    AABB2 m_bounds;
    AABB2 m_uvs;
    Texture* m_texture;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectWireBounds : public DRObjectSimpleOneColor
{
public:
    DRObjectWireBounds(std::vector<Vertex_PCU> const& verts, std::vector<unsigned int> const& indices,
        Rgba8 const& startColor, Rgba8 const& endColor, double duration, eDebugRenderMode mode, bool isWorldSpace);
};

//////////////////////////////////////////////////////////////////////////
class DRObjectWireMesh : public DRObjectSimpleOneColor
{
public:
    DRObjectWireMesh(Mat44 const& transform, GPUMesh* mesh, Rgba8 const& startColor, Rgba8 const& endColor, 
        double duration, eDebugRenderMode mode);

    void UpdateTint();

public:
    Rgba8 m_curTint;
    Mat44 m_transform;
    GPUMesh* m_mesh = nullptr;
};

//////////////////////////////////////////////////////////////////////////
class DRObjectText : public DebugRenderObject
{
public:
    DRObjectText(std::string const& text, float size, Rgba8 const& startColor, Rgba8 const& endColor,
        bool isFacingCam, Vec3 const& center, Vec2 const& pivot, double duration, eDebugRenderMode mode, 
        bool isWorldSpace, Vec2 const& offset = Vec2::ZERO, Mat44 const& basis = Mat44::IDENTITY);

    DRObjectText(std::string const& text, float size, AABB2 const& bounds, Vec2 const& alignment, Vec3 const& center,
        Rgba8 const& startColor, Rgba8 const& endColor, bool isFacingCam, float duration, eDebugRenderMode mode, 
        bool isWorldSpace, Mat44 const& basis = Mat44::IDENTITY);

    virtual void AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices) override;
    virtual const Texture* GetTexture() const override;

private:
    std::string m_text;
    float m_size = 1.f;
    Rgba8 m_startColor;
    Rgba8 m_endColor;
    bool m_isFacingCamera = false;
    bool m_isCenterValid = true;
    Vec3 m_center;  //ui is 0 ~ 1, world is actual pos
    Vec2 m_offset; //just for screen
    Vec2 m_pivot;
};

static constexpr int STRING_STACK_LENGTH = 2048;

//------------------------------------------------------------------------
// Class Method definitions
//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
DebugRenderSystem::DebugRenderSystem(RenderContext* ctx)
    : m_context(ctx)
{
    m_camera = new Camera();
    m_clock = new Clock();
    m_font = ctx->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
}

//////////////////////////////////////////////////////////////////////////
DebugRenderSystem::~DebugRenderSystem()
{
    ClearRenderObjects();
    delete m_camera;
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSystem::AppendNewDebugRenderObject(DebugRenderObject* newObject)
{
    for (size_t i = 0; i < m_objects.size(); i++) {
        if (m_objects[i] == nullptr) {
            m_objects[i] = newObject;
            return;
        }
    }
    m_objects.push_back(newObject);
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSystem::CleanDeadObjects()
{
    for (size_t i = 0; i < m_objects.size(); i++) {
        DebugRenderObject* object = m_objects[i];
        if (object != nullptr && object->IsReadyToBeCulled()) {
            delete object;
            m_objects[i] = nullptr;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSystem::ClearRenderObjects()
{
    for (size_t i = 0; i < m_objects.size(); i++) {
        if (m_objects[i] != nullptr) {
            delete m_objects[i];
            m_objects[i] = nullptr;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSystem::SetDebugRenderEnabled(bool isEnabled)
{
    m_enabled = isEnabled;
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSystem::SetScreenHeight(float newHeight)
{
    m_screenHeight = newHeight;
    m_defaultLineWidth = .005f * m_screenHeight;
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSystem::SetWorldAxisConvention(AxisConvention newConvention)
{
    m_worldAxisConvention = newConvention;
}

//////////////////////////////////////////////////////////////////////////
Mat44 DebugRenderSystem::GetWorldAxisMatrix() const
{
    return GetAxisMatrixFromConvention(m_worldAxisConvention);
}

static DebugRenderSystem* sDebugRenderSystem = nullptr;

//////////////////////////////////////////////////////////////////////////
DebugRenderObject::DebugRenderObject(double duration, eDebugRenderMode mode, bool isWorldSpace)
    : m_isWorldSpace(isWorldSpace)
    , m_duration(duration)
    , m_mode(mode)
{
    m_timer = new Timer();
    m_timer->SetTimerSeconds(sDebugRenderSystem->GetClock(), duration);
}

//////////////////////////////////////////////////////////////////////////
bool DebugRenderObject::IsReadyToBeCulled() const
{
    return m_timer->IsRunning() &&  m_timer->HasElapsed();
}

//////////////////////////////////////////////////////////////////////////
DRObjectSimpleOneColor::DRObjectSimpleOneColor(Rgba8 const& startColor, Rgba8 const& endColor, double duration, eDebugRenderMode mode, bool isWorldSpace)
    : DebugRenderObject(duration, mode, isWorldSpace)
    , m_startColor(startColor)
    , m_endColor(endColor)
{
}

//////////////////////////////////////////////////////////////////////////
void DRObjectSimpleOneColor::AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices)
{
    double remaining = m_timer->GetRemainingSeconds();
    Rgba8 newColor = m_startColor;
    if (m_duration > 0.0 && m_startColor != m_endColor) {
        newColor = LerpAsHSL(m_startColor, m_endColor, (float)(remaining / m_duration));
    }

    unsigned int vertsStartIdx = (unsigned int)verts.size();
    for (size_t i = 0; i < m_indices.size(); i++) {
        indices.push_back(m_indices[i] + vertsStartIdx);
    }

    for (size_t i = 0; i < m_verts.size(); i++) {
        Vertex_PCU v = m_verts[i];
        v.tint = newColor;
        verts.push_back(v);
    }
}


//////////////////////////////////////////////////////////////////////////
DRObjectPoint::DRObjectPoint(Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, double duration, eDebugRenderMode mode, bool isWorldSpace)
    : DRObjectSimpleOneColor(startColor,endColor,duration, mode, isWorldSpace)
    , m_position(pos)
    , m_size(size)
{
    if (isWorldSpace) {
        AppendIndexedVertexesForUVSphere(m_verts, m_indices, pos, size, 16, 16, startColor, Vec2::ZERO,Vec2::ZERO);
    }
    else {
        AppendIndexedVertexesForDisc2D(m_verts, m_indices, Vec2(pos.x, pos.y), size, Rgba8::WHITE, 16);
    }
}

//////////////////////////////////////////////////////////////////////////
DRObjectLine::DRObjectLine(Vec3 p0, Rgba8 const& startColor0, Rgba8 const& endColor0, Vec3 p1, Rgba8 const& startColor1, Rgba8 const& endColor1, double duration, eDebugRenderMode mode, bool isWorldSpace)
    : DebugRenderObject(duration, mode, isWorldSpace)
    , m_start(p0)
    , m_end(p1)
    , m_startColor0(startColor0)
    , m_startColor1(startColor1)
    , m_endColor0(endColor0)
    , m_endColor1(endColor1)
{
    float defaultWidth = sDebugRenderSystem->GetDefaultLineWidth();
    if (isWorldSpace) {        
        AppendIndexedVertexesForCylinder(m_verts, m_indices, p0, p1, defaultWidth * .001f, sLineCuts, startColor0);
    }
    else {
        AppendIndexedVertexesForLine2D(m_verts, m_indices, Vec2(p0.x, p0.y), Vec2(p1.x, p1.y), defaultWidth, startColor0);
    }
}

//////////////////////////////////////////////////////////////////////////
void DRObjectLine::AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices)
{
    double remaining = m_timer->GetRemainingSeconds();
    Rgba8 color0 = m_startColor0;
    Rgba8 color1 = m_startColor1;
    if (m_duration > 0.0) {
        float fraction = (float)(remaining / m_duration);
        if (m_startColor0 != m_endColor0) {
            color0 = LerpAsHSL(m_startColor0, m_endColor0, fraction);
        }
        if (m_startColor1 != m_endColor1) {
            color1 = LerpAsHSL(m_startColor1, m_endColor1, fraction);
        }
    }

    int step = m_isWorldSpace ? sLineCuts+1 : 2;
    unsigned int vertsStartIdx = (unsigned int)verts.size();
    for (size_t i = 0; i < m_indices.size(); i++) {
        indices.push_back(vertsStartIdx + m_indices[i]);
    }
    for (size_t i = 0; i < step; i++) {
        Vertex_PCU v0 = m_verts[i];
        v0.tint = color0;
        verts.push_back(v0);
    }
    for (size_t i = 0; i < step; i++) {
        Vertex_PCU v1 = m_verts[i + step];
        v1.tint = color1;
        verts.push_back(v1);
    }
}

//////////////////////////////////////////////////////////////////////////
DRObjectArrow::DRObjectArrow(Vec3 p0, Rgba8 const& startColor0, Rgba8 const& endColor0, Vec3 p1, Rgba8 const& startColor1, Rgba8 const& endColor1, double duration, eDebugRenderMode mode, bool isWorldSpace)
    :DebugRenderObject(duration,mode,isWorldSpace)
    ,m_start(p0)
    ,m_end(p1)
    ,m_startColor0(startColor0)
    ,m_startColor1(startColor1)
    ,m_endColor0(endColor0)
    ,m_endColor1(endColor1)
{
    float width = sDebugRenderSystem->GetDefaultLineWidth() * .1f;    
    if (isWorldSpace) {
        width = .01f;
        AppendIndexedVertexesForArrow(m_verts, m_indices, p0, p1, width, width * 2.f, sArrowCuts, startColor0);
    }
    else {
        width *= 30.f;
        Vec3 middle = (p0 + p1) * .5f;
        Vec3 direction = (p1 - p0).GetNormalized();
        Vec2 verticle = Vec2(-direction.y, direction.x) * width;
        AppendIndexedVertexesForQuaterPolygon2D(m_verts, m_indices,
            p0 + verticle, p0 - verticle, middle - verticle, middle + verticle, startColor0);
        AppendIndexedVertexesForTriangle2D(m_verts, m_indices,
            middle + verticle * 3.f, middle - verticle * 3.f, p1, startColor0);
    }
}

//////////////////////////////////////////////////////////////////////////
void DRObjectArrow::AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices)
{
    double remaining = m_timer->GetRemainingSeconds();
    Rgba8 color0 = m_startColor0;
    Rgba8 colorMiddle = LerpAsHSL(m_startColor0, m_startColor1, .5f);
    Rgba8 color1 = m_startColor1;
    if (m_duration > 0.0) {
        float fraction = (float)(remaining / m_duration);
        if (m_startColor0 != m_endColor0) {
            color0 = LerpAsHSL(m_startColor0, m_endColor0, fraction);
        }
        if (m_startColor1 != m_endColor1) {
            color1 = LerpAsHSL(m_startColor1, m_endColor1, fraction);
        }
        Rgba8 endColorMiddle = LerpAsHSL(m_endColor0, m_endColor1, .5f);
        if (colorMiddle != endColorMiddle) {
            colorMiddle = LerpAsHSL(colorMiddle, endColorMiddle, fraction);
        }
    }

    int step = m_isWorldSpace ? sArrowCuts + 1 : 2;
    unsigned int vertsStartIdx = (unsigned int)verts.size();
    for (size_t i = 0; i < m_indices.size(); i++) {
        indices.push_back(vertsStartIdx + m_indices[i]);
    }
    for (int i = 0; i < step; i++) {
        Vertex_PCU v0 = m_verts[i];
        v0.tint = color0;
        verts.push_back(v0);
    }
    for (int i = step; i < 3 * step; i++) {
        Vertex_PCU vMiddle = m_verts[i];
        vMiddle.tint = colorMiddle;
        verts.push_back(vMiddle);
    }
    for (int i = 3*step; i < (int)m_verts.size(); i++) {
        Vertex_PCU v1 = m_verts[i];
        v1.tint = color1;
        verts.push_back(v1);
    }
}

//////////////////////////////////////////////////////////////////////////
DRObjectBasis::DRObjectBasis(Mat44 const& basis, Rgba8 const& startTint, Rgba8 const& endTint, double duration, eDebugRenderMode mode, bool isWorldSpace)
    :DebugRenderObject(duration,mode,isWorldSpace)
    ,m_basis(basis)
    ,m_startTint(startTint)
    ,m_endTint(endTint)
{
    float width = sDebugRenderSystem->GetDefaultLineWidth() * .006f;
    if (isWorldSpace) {
        AppendIndexedVertexesForArrow(m_verts, m_indices, Vec3::ZERO, Vec3(1.f, 0.f, 0.f), width, width * 2.f, sArrowCuts, Rgba8::RED);
        AppendIndexedVertexesForArrow(m_verts, m_indices, Vec3::ZERO, Vec3(0.f, 1.f, 0.f), width, width * 2.f, sArrowCuts, Rgba8::GREEN);
        AppendIndexedVertexesForArrow(m_verts, m_indices, Vec3::ZERO, Vec3(0.f, 0.f, 1.f), width, width * 2.f, sArrowCuts, Rgba8::BLUE);

        TransformVertexArray((int)m_verts.size(), &m_verts[0], basis);
    }
}

//////////////////////////////////////////////////////////////////////////
void DRObjectBasis::AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices)
{
    double remaining = m_timer->GetRemainingSeconds();
    Rgba8 newTint = m_startTint;
    if (m_duration > 0.0 && m_startTint!=m_endTint) {
        newTint = LerpAsHSL(m_startTint, m_endTint, (float)(remaining / m_duration));
    }

    unsigned int vertsStartIdx = (unsigned int)verts.size();
    for (size_t i = 0; i < m_indices.size(); i++) {
        indices.push_back(m_indices[i] + vertsStartIdx);
    }

    Mat44 axisMat = sDebugRenderSystem->GetWorldAxisMatrix();
    for (size_t i = 0; i < m_verts.size(); i++) {
        Vertex_PCU v = m_verts[i];
        //v.position = axisMat.TransformPosition3D(v.position);
        v.tint = v.tint.GetTintedColor(newTint);
        verts.push_back(v);
    }
}

//////////////////////////////////////////////////////////////////////////
DRObjectScreenBasis::DRObjectScreenBasis(Vec2 const& origin, Mat44 const& basis, Rgba8 const& startTint, Rgba8 const& endTint, float duration)
    : DebugRenderObject(duration, DEBUG_RENDER_ALWAYS, true)
    , m_screenOrigin(origin)
    , m_startTint(startTint)
    , m_endTint(endTint)
{
    float width = sDebugRenderSystem->GetDefaultLineWidth() * .01f;
    
    AppendIndexedVertexesForArrow(m_verts, m_indices, Vec3::ZERO, Vec3(.8f, 0.f, 0.f), width, width * 2.f, sArrowCuts, Rgba8::RED);
    AppendIndexedVertexesForArrow(m_verts, m_indices, Vec3::ZERO, Vec3(0.f, .8f, 0.f), width, width * 2.f, sArrowCuts, Rgba8::GREEN);
    AppendIndexedVertexesForArrow(m_verts, m_indices, Vec3::ZERO, Vec3(0.f, 0.f, .8f), width, width * 2.f, sArrowCuts, Rgba8::BLUE);

    TransformVertexArray((int)m_verts.size(), &m_verts[0], basis);    
}

void DRObjectScreenBasis::AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices)
{
    double remaining = m_timer->GetRemainingSeconds();
    Rgba8 newTint = m_startTint;
    if (m_duration > 0.0 && m_startTint != m_endTint) {
        newTint = LerpAsHSL(m_startTint, m_endTint, (float)(remaining / m_duration));
    }

    unsigned int vertsStartIdx = (unsigned int)verts.size();
    for (size_t i = 0; i < m_indices.size(); i++) {
        indices.push_back(m_indices[i] + vertsStartIdx);
    }

    Camera* cam = sDebugRenderSystem->GetCamera();
    Vec3 pos = cam->ClientToWorldPosition(m_screenOrigin, .985f);

    Mat44 axisMat = sDebugRenderSystem->GetWorldAxisMatrix();
    for (size_t i = 0; i < m_verts.size(); i++) {
        Vertex_PCU v = m_verts[i];
        //v.position = axisMat.TransformPosition3D(v.position);
        v.tint = v.tint.GetTintedColor(newTint);
        v.position += pos;
        verts.push_back(v);
    }
}

//////////////////////////////////////////////////////////////////////////
DRObjectQuad::DRObjectQuad(Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Vec3 const& p3, Rgba8 const& startColor, Rgba8 const& endColor, double duration, eDebugRenderMode mode, bool isWorldSpace)
    : DRObjectSimpleOneColor(startColor,endColor,duration,mode,isWorldSpace)
    ,m_p0(p0)
    ,m_p1(p1)
    ,m_p2(p2)
    ,m_p3(p3)
{
    if (isWorldSpace) {
        m_isTextured = true;
    }

    AppendIndexedVertexesForQuaterPolygon2D(m_verts, m_indices, p0, p1, p2, p3, startColor);
}

//////////////////////////////////////////////////////////////////////////
DRObjectTexturedQuad::DRObjectTexturedQuad(AABB2 const& bounds, Texture* tex, AABB2 const& uvs, Rgba8 const& startTint, Rgba8 const& endTint, double duration, eDebugRenderMode mode)
    : DRObjectSimpleOneColor(startTint,endTint,duration, mode, false)
    , m_bounds(bounds)
    , m_texture(tex)
    , m_uvs(uvs)
{
    m_isTextured = true;

    Vec2 mins = bounds.mins;
    Vec2 maxs = bounds.maxs;
    AppendIndexedVertexesForQuaterPolygon2D(m_verts, m_indices, mins, Vec2(maxs.x, mins.y), maxs, Vec2(mins.x, maxs.y),
        startTint, uvs.mins, uvs.maxs);
}

//////////////////////////////////////////////////////////////////////////
DRObjectWireBounds::DRObjectWireBounds(std::vector<Vertex_PCU> const& verts, std::vector<unsigned int> const& indices, Rgba8 const& startColor, Rgba8 const& endColor, double duration, eDebugRenderMode mode, bool isWorldSpace)
    : DRObjectSimpleOneColor(startColor,endColor,duration,mode,isWorldSpace)
{
    m_verts = verts;
    m_indices = indices;
    m_type = DROBJECT_WIRE_BOUNDS;
}

//////////////////////////////////////////////////////////////////////////
DRObjectWireMesh::DRObjectWireMesh(Mat44 const& transform, GPUMesh* mesh, Rgba8 const& startColor, Rgba8 const& endColor, double duration, eDebugRenderMode mode)
    : DRObjectSimpleOneColor(startColor, endColor, duration, mode, true)
    , m_mesh(mesh)
    , m_transform(transform)
{
    m_type = DROBJECT_MESH;
}

//////////////////////////////////////////////////////////////////////////
void DRObjectWireMesh::UpdateTint()
{
    double remaining = m_timer->GetRemainingSeconds();
    m_curTint = m_startColor;
    if (m_duration > 0.0 && m_startColor != m_endColor) {
        m_curTint = LerpAsHSL(m_startColor, m_endColor, (float)(remaining / m_duration));
    }
}

//////////////////////////////////////////////////////////////////////////
DRObjectText::DRObjectText(std::string const& text, float size, Rgba8 const& startColor, Rgba8 const& endColor, 
    bool isFacingCam, Vec3 const& center, Vec2 const& pivot, double duration, eDebugRenderMode mode, bool isWorldSpace, 
    Vec2 const& offset /*= Vec2::ZERO*/, Mat44 const& basis /*= Mat44::IDENTITY*/)
    : DebugRenderObject(duration,mode,isWorldSpace)
    ,m_text(text)
    ,m_size(size)
    ,m_startColor(startColor)
    ,m_endColor(endColor)
    ,m_isFacingCamera(isFacingCam)
    ,m_center(center)
    ,m_pivot(pivot)
    ,m_offset(offset)
{
    m_type = DROBJECT_TEXT;
    m_isTextured = true;
    BitmapFont* sFont = sDebugRenderSystem->GetFont();
    Vec2 textDim = sFont->GetDimensionsForText2D(size, text, 1.f);
    sFont->AddVertsForTextInBox2D(m_verts, AABB2(-textDim * pivot, textDim * (Vec2::ONE - pivot)), size, text, startColor);

    if (isWorldSpace) {        
        Mat44 trans;
        if (isFacingCam) {
            trans.SetTranslation3D(center);
        }
        else {
            trans = basis;
        }
        TransformVertexArray((int)m_verts.size(), &m_verts[0], trans);
    }

    for (unsigned int i = 0; i < (unsigned int)m_verts.size(); i++) {
        m_indices.push_back(i);
    }
}

//////////////////////////////////////////////////////////////////////////
DRObjectText::DRObjectText(std::string const& text, float size, AABB2 const& bounds, Vec2 const& alignment, Vec3 const& center, Rgba8 const& startColor, Rgba8 const& endColor,
    bool isFacingCam, float duration, eDebugRenderMode mode, bool isWorldSpace, Mat44 const& basis)
    : DebugRenderObject(duration, mode, isWorldSpace)
    , m_text(text)
    , m_size(size)
    , m_isFacingCamera(isFacingCam)
    , m_startColor(startColor)
    , m_endColor(endColor)
    , m_center(center)
{
    m_type = DROBJECT_TEXT;
    m_isTextured = true;
    BitmapFont* sFont = sDebugRenderSystem->GetFont();
    Vec2 textDim = sFont->GetDimensionsForText2D(size, text, 1.f);

    //shrink to fit
    Vec2 newAlign = alignment;
    newAlign.x = Clamp(alignment.x, 0.f, 1.f);
    newAlign.y = Clamp(alignment.y, 0.f, 1.f);
    AABB2 newBounds = AABB2(bounds.mins, bounds.mins + textDim);
    AABB2 oldBounds = bounds;
    oldBounds.FitInBoundsAndResize(newBounds);
    Vec2 newDim = oldBounds.GetDimensions();
    Vec2 oldDim = bounds.GetDimensions();
    float newSize = size;
    if (newDim.x > oldDim.x) {
        newSize *= oldDim.x / newDim.x;
    }

    sFont->AddVertsForTextInBox2D(m_verts, bounds, newSize, text, startColor,1.f,newAlign);

    if (isWorldSpace) {
        if (!isFacingCam) {
            TransformVertexArray((int)m_verts.size(), &m_verts[0], basis);
        }
    }
    else {
        m_offset = bounds.GetCenter();
        m_isCenterValid = false;
    }

    for (unsigned int i = 0; i < (unsigned int)m_verts.size(); i++) {
        m_indices.push_back(i);
    }
}

//////////////////////////////////////////////////////////////////////////
void DRObjectText::AppendToVerts(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices)
{
    Mat44 basis;
    Camera* cam = sDebugRenderSystem->GetCamera();
    if (m_isWorldSpace && m_isFacingCamera) {
            basis = cam->GetCameraModel();
            basis.SetTranslation3D(m_center);
    }
    if (!m_isWorldSpace) {
        Vec2 pos;
        if (m_isCenterValid) {
            AABB2 bounds = cam->GetBounds();
            pos = bounds.GetPointAtUV(Vec2(m_center.x, m_center.y));
        }
        basis.SetTranslation2D(m_offset + pos);
    }

    double remaining = m_timer->GetRemainingSeconds();
    Rgba8 newColor = m_startColor;
    if (m_duration > 0.0 && m_startColor != m_endColor) {
        newColor = LerpAsHSL(m_startColor, m_endColor, (float)(remaining / m_duration));
    }

    unsigned int vertsStartIdx = (unsigned int)verts.size();
    for (size_t i = 0; i < m_indices.size(); i++) {
        indices.push_back(m_indices[i] + vertsStartIdx);
    }

    for (size_t i = 0; i < m_verts.size(); i++) {
        Vertex_PCU v = m_verts[i];
        v.tint = newColor;
        v.position = basis.TransformPosition3D(v.position);
        verts.push_back(v);
    }
}

//////////////////////////////////////////////////////////////////////////
const Texture* DRObjectText::GetTexture() const
{
    return sDebugRenderSystem->GetFont()->GetTexture();
}

//------------------------------------------------------------------------
// Debug Render Utilities
//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
void DebugRenderSystemStartup(RenderContext* ctx)
{
    sDebugRenderSystem = new DebugRenderSystem(ctx);
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSystemShutdown()
{
    delete sDebugRenderSystem;
}

//////////////////////////////////////////////////////////////////////////
bool IsDebugRenderingEnabled()
{
    return sDebugRenderSystem->IsEnabled();
}

//////////////////////////////////////////////////////////////////////////
void EnableDebugRendering()
{
    sDebugRenderSystem->SetDebugRenderEnabled(true);
}

//////////////////////////////////////////////////////////////////////////
void DisableDebugRendering()
{
    sDebugRenderSystem->SetDebugRenderEnabled(false);
}

//////////////////////////////////////////////////////////////////////////
void ClearDebugRendering()
{
    sDebugRenderSystem->ClearRenderObjects();
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderBeginFrame()
{
}

//////////////////////////////////////////////////////////////////////////
static void DebugRenderWorldNormal(RenderContext* ctx, Camera* cam)
{
    std::vector<Vertex_PCU> depthVerts;
    std::vector<Vertex_PCU> xVerts;
    std::vector<Vertex_PCU> alwaysVerts;
    std::vector<unsigned int> depthIndices;
    std::vector<unsigned int> xIndices;
    std::vector<unsigned int> alwaysIndices;
    GPUMesh* depthMesh = new GPUMesh(ctx);
    GPUMesh* xMesh = new GPUMesh(ctx);
    GPUMesh* alwaysMesh = new GPUMesh(ctx);
    std::vector<DebugRenderObject*> objects = sDebugRenderSystem->GetObjects();

    for (size_t i = 0; i < objects.size(); i++) {
        DebugRenderObject* ob = objects[i];
        if (ob == nullptr || !ob->IsWorldSpace()) {
            continue;
        }

        eDebugRenderObjectType type = ob->GetType();
        if (type == DROBJECT_WIRE_BOUNDS || type==DROBJECT_TEXT || type == DROBJECT_MESH) {
            continue;
        }

        eDebugRenderMode mode = ob->GetMode();
        if (mode == DEBUG_RENDER_USE_DEPTH) {
            ob->AppendToVerts(depthVerts, depthIndices);
        }
        else if (mode == DEBUG_RENDER_ALWAYS) {
            ob->AppendToVerts(alwaysVerts, alwaysIndices);
        }
        else if (mode == DEBUG_RENDER_XRAY) {
            ob->AppendToVerts(xVerts, xIndices);
        }
    }
    if (depthVerts.size() > 0) {
        depthMesh->UpdateVertices((unsigned int)depthVerts.size(), &depthVerts[0]);
        depthMesh->UpdateIndices((unsigned int)depthIndices.size(), &depthIndices[0]);
    }
    if (xVerts.size() > 0) {
        xMesh->UpdateVertices((unsigned int)xVerts.size(), &xVerts[0]);
        xMesh->UpdateIndices((unsigned int)xIndices.size(), &xIndices[0]);
    }
    if (alwaysVerts.size() > 0) {
        alwaysMesh->UpdateVertices((unsigned int)alwaysVerts.size(), &alwaysVerts[0]);
        alwaysMesh->UpdateIndices((unsigned int)alwaysIndices.size(), &alwaysIndices[0]);
    }

    ctx->BeginCamera(cam);

    //depth greater pass    
    ctx->EnableDepth(COMPARE_FUNC_GREATER, false);
    //always
    ctx->DrawMesh(alwaysMesh);
    //xray
    ctx->SetModelData(Mat44::IDENTITY, sXrayTint);
    ctx->DrawMesh(xMesh);

    //normal pass    
    ctx->EnableDepth(COMPARE_FUNC_LEQUAL, true);
    ctx->SetModelData(Mat44::IDENTITY, Rgba8::WHITE);
    ctx->DrawMesh(depthMesh);
    ctx->DrawMesh(alwaysMesh);
    ctx->DrawMesh(xMesh);

    ctx->EndCamera(cam);

    delete depthMesh;
    delete alwaysMesh;
    delete xMesh;
}

//////////////////////////////////////////////////////////////////////////
static void DebugRenderWorldWire(RenderContext* ctx, Camera* cam)
{
    std::vector<Vertex_PCU> depthVerts;
    std::vector<Vertex_PCU> xVerts;
    std::vector<Vertex_PCU> alwaysVerts;
    std::vector<unsigned int> depthIndices;
    std::vector<unsigned int> xIndices;
    std::vector<unsigned int> alwaysIndices;
    GPUMesh* depthMesh = new GPUMesh(ctx);
    GPUMesh* xMesh = new GPUMesh(ctx);
    GPUMesh* alwaysMesh = new GPUMesh(ctx);
    std::vector<DebugRenderObject*> objects = sDebugRenderSystem->GetObjects();

    for (size_t i = 0; i < objects.size(); i++) {
        DebugRenderObject* ob = objects[i];
        if (ob == nullptr || !ob->IsWorldSpace()) {
            continue;
        }

        eDebugRenderObjectType type = ob->GetType();
        if (type != DROBJECT_WIRE_BOUNDS) {
            continue;
        }

        eDebugRenderMode mode = ob->GetMode();
        if (mode == DEBUG_RENDER_USE_DEPTH) {
            ob->AppendToVerts(depthVerts, depthIndices);
        }
        else if (mode == DEBUG_RENDER_ALWAYS) {
            ob->AppendToVerts(alwaysVerts, alwaysIndices);
        }
        else if (mode == DEBUG_RENDER_XRAY) {
            ob->AppendToVerts(xVerts, xIndices);
        }
    }
    if (depthVerts.size() > 0) {
        depthMesh->UpdateVertices((unsigned int)depthVerts.size(), &depthVerts[0]);
        depthMesh->UpdateIndices((unsigned int)depthIndices.size(), &depthIndices[0]);
    }
    if (xVerts.size() > 0) {
        xMesh->UpdateVertices((unsigned int)xVerts.size(), &xVerts[0]);
        xMesh->UpdateIndices((unsigned int)xIndices.size(), &xIndices[0]);
    }
    if (alwaysVerts.size() > 0) {
        alwaysMesh->UpdateVertices((unsigned int)alwaysVerts.size(), &alwaysVerts[0]);
        alwaysMesh->UpdateIndices((unsigned int)alwaysIndices.size(), &alwaysIndices[0]);
    }

    ctx->BeginCamera(cam);
    ctx->SetFillMode(eFillMode::FILL_WIREFRAME);

    //depth greater pass    
    ctx->EnableDepth(COMPARE_FUNC_GREATER, false);
    //always
    ctx->DrawMesh(alwaysMesh);
    //xray
    ctx->SetModelData(Mat44::IDENTITY, sXrayTint);
    ctx->DrawMesh(xMesh);

    //normal pass    
    ctx->EnableDepth(COMPARE_FUNC_LEQUAL, true);
    ctx->SetModelData(Mat44::IDENTITY, Rgba8::WHITE);
    ctx->DrawMesh(depthMesh);
    ctx->DrawMesh(alwaysMesh);
    ctx->DrawMesh(xMesh);

    ctx->EndCamera(cam);

    delete depthMesh;
    delete alwaysMesh;
    delete xMesh;
}

//////////////////////////////////////////////////////////////////////////
static void DebugRenderWorldMesh(RenderContext* ctx, Camera* cam)
{
    std::vector<DebugRenderObject*> objects = sDebugRenderSystem->GetObjects();

    std::vector<DRObjectWireMesh*> alwaysMeshes;
    std::vector<DRObjectWireMesh*> xMeshes;
    std::vector<DRObjectWireMesh*> depthMeshes;

    ctx->BeginCamera(cam);
    ctx->SetFillMode(FILL_WIREFRAME);
    ctx->SetCullMode(CULL_NONE);

    for (size_t i = 0; i < objects.size(); i++) {
        DebugRenderObject* ob = objects[i];
        if (ob == nullptr || !ob->IsWorldSpace()) {
            continue;
        }

        eDebugRenderObjectType type = ob->GetType();
        if (type != DROBJECT_MESH) {
            continue;
        }

        DRObjectWireMesh* mesh = (DRObjectWireMesh*)ob;
        mesh->UpdateTint();
        switch (ob->GetMode())
        {
            case DEBUG_RENDER_ALWAYS:    alwaysMeshes.push_back(mesh); break;
            case DEBUG_RENDER_USE_DEPTH: depthMeshes.push_back(mesh); break;
            case DEBUG_RENDER_XRAY:      xMeshes.push_back(mesh); break;
        }
    }

    //depth greater pass    
    ctx->EnableDepth(COMPARE_FUNC_GREATER, false);
    //always
    for (DRObjectWireMesh* mesh : alwaysMeshes) {
        ctx->SetModelData(mesh->m_transform, mesh->m_curTint);
        ctx->DrawMesh(mesh->m_mesh);
    }
    //xray
    for(DRObjectWireMesh* xMesh: xMeshes){
        ctx->SetModelData(xMesh->m_transform, Rgba8(255, 0, 0, 128));
        ctx->DrawMesh(xMesh->m_mesh);
    }

    //normal pass    
    ctx->EnableDepth(COMPARE_FUNC_LEQUAL, true);
    //always
    for (DRObjectWireMesh* mesh : alwaysMeshes) {
        ctx->SetModelData(mesh->m_transform, mesh->m_curTint);
        ctx->DrawMesh(mesh->m_mesh);
    }
    //xray
    for (DRObjectWireMesh* xMesh : xMeshes) {
        ctx->SetModelData(xMesh->m_transform, xMesh->m_curTint);
        ctx->DrawMesh(xMesh->m_mesh);
    }
    //depth
    for (DRObjectWireMesh* dMesh : depthMeshes) {
        ctx->SetModelData(dMesh->m_transform, dMesh->m_curTint);
        ctx->DrawMesh(dMesh->m_mesh);
    }

    ctx->EndCamera(cam);
}

static void DebugRenderWorldText(RenderContext* ctx, Camera* cam)
{
    std::vector<Vertex_PCU> depthVerts;
    std::vector<Vertex_PCU> xVerts;
    std::vector<Vertex_PCU> alwaysVerts;
    std::vector<unsigned int> depthIndices;
    std::vector<unsigned int> xIndices;
    std::vector<unsigned int> alwaysIndices;
    GPUMesh* depthMesh = new GPUMesh(ctx);
    GPUMesh* xMesh = new GPUMesh(ctx);
    GPUMesh* alwaysMesh = new GPUMesh(ctx);
    std::vector<DebugRenderObject*> objects = sDebugRenderSystem->GetObjects();

    for (size_t i = 0; i < objects.size(); i++) {
        DebugRenderObject* ob = objects[i];
        if (ob == nullptr || !ob->IsWorldSpace()) {
            continue;
        }

        eDebugRenderObjectType type = ob->GetType();
        if (type != DROBJECT_TEXT) {
            continue;
        }

        eDebugRenderMode mode = ob->GetMode();
        if (mode == DEBUG_RENDER_USE_DEPTH) {
            ob->AppendToVerts(depthVerts, depthIndices);
        }
        else if (mode == DEBUG_RENDER_ALWAYS) {
            ob->AppendToVerts(alwaysVerts, alwaysIndices);
        }
        else if (mode == DEBUG_RENDER_XRAY) {
            ob->AppendToVerts(xVerts, xIndices);
        }
    }
    if (depthVerts.size() > 0) {
        depthMesh->UpdateVertices((unsigned int)depthVerts.size(), &depthVerts[0]);
        depthMesh->UpdateIndices((unsigned int)depthIndices.size(), &depthIndices[0]);
    }
    if (xVerts.size() > 0) {
        xMesh->UpdateVertices((unsigned int)xVerts.size(), &xVerts[0]);
        xMesh->UpdateIndices((unsigned int)xIndices.size(), &xIndices[0]);
    }
    if (alwaysVerts.size() > 0) {
        alwaysMesh->UpdateVertices((unsigned int)alwaysVerts.size(), &alwaysVerts[0]);
        alwaysMesh->UpdateIndices((unsigned int)alwaysIndices.size(), &alwaysIndices[0]);
    }

    ctx->BeginCamera(cam);
    ctx->BindDiffuseTexture(sDebugRenderSystem->GetFont()->GetTexture());

    //depth greater pass    
    ctx->EnableDepth(COMPARE_FUNC_GREATER, false);
    //always
    ctx->DrawMesh(alwaysMesh);
    //xray
    ctx->SetModelData(Mat44::IDENTITY, sXrayTint);
    ctx->DrawMesh(xMesh);

    //normal pass    
    ctx->EnableDepth(COMPARE_FUNC_LEQUAL, true);
    ctx->SetModelData(Mat44::IDENTITY, Rgba8::WHITE);
    ctx->DrawMesh(depthMesh);
    ctx->DrawMesh(alwaysMesh);
    ctx->DrawMesh(xMesh);

    ctx->EndCamera(cam);

    delete depthMesh;
    delete alwaysMesh;
    delete xMesh;
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderWorldToCamera(Camera* cam)
{
    if (!sDebugRenderSystem->IsEnabled()) {
        return;
    }

    RenderContext* ctx = sDebugRenderSystem->GetRenderContext();
    Camera* debugCam = sDebugRenderSystem->GetCamera();
    debugCam->SetColorTarget(nullptr);
    debugCam->SetDepthStencilTarget(cam->GetDepthStencilTarget());
    debugCam->SetViewMatrix(cam->GetView());
    debugCam->SetProjectionMatrix(cam->GetProjectionMatrix());
    debugCam->SetClearMode(eCameraClearBitFlag::CLEAR_NONE, Rgba8::WHITE, cam->GetClearDepth());
    sDebugRenderSystem->SetWorldAxisConvention(cam->GetAxisConvention());

    DebugRenderWorldNormal(ctx, debugCam);
    DebugRenderWorldWire(ctx, debugCam);
    DebugRenderWorldMesh(ctx, debugCam);
    DebugRenderWorldText(ctx, debugCam);
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderScreenTo(Texture* tex)
{
    if (!sDebugRenderSystem->IsEnabled()) {
        return;
    }

    IntVec2 texSize = tex->GetTextureSize();
    Vec2 textureSize = Vec2((float)texSize.x, (float)texSize.y);

    RenderContext* ctx = sDebugRenderSystem->GetRenderContext();
    Camera* debugCam = sDebugRenderSystem->GetCamera();
    debugCam->SetColorTarget(tex);
    debugCam->SetOrthoView(-textureSize,textureSize);
    debugCam->SetProjectionOrthographic(sDebugRenderSystem->GetScreenHeight());
    //debugCam->SetClearMode(eCameraClearBitFlag::CLEAR_NONE, Rgba8::WHITE);

    ctx->BeginCamera(debugCam);
    ctx->DisableDepth();
    std::vector<Vertex_PCU> verts;
    std::vector<unsigned int> indices;
    GPUMesh* mesh = new GPUMesh(ctx);

    std::vector<DebugRenderObject*> objects = sDebugRenderSystem->GetObjects();

    //non textured
    for (size_t i = 0; i < objects.size(); i++) {
        DebugRenderObject* ob = objects[i];
        if (ob == nullptr || ob->IsWorldSpace() || ob->IsTextured()) {
            continue;
        }

        ob->AppendToVerts(verts, indices);
    }
    if (verts.size() > 0 && indices.size() > 0) {
        mesh->UpdateVertices((unsigned int)verts.size(), &verts[0]);
        mesh->UpdateIndices((unsigned int)indices.size(), &indices[0]);
        ctx->DrawMesh(mesh);
    }

    //textured
    for (size_t i = 0; i < objects.size(); i++) {
        DebugRenderObject* ob = objects[i];
        if (ob == nullptr || ob->IsWorldSpace() || !ob->IsTextured()) {
            continue;
        }

        std::vector<Vertex_PCU> tempVerts;
        std::vector<unsigned int> tempIndices;
        ob->AppendToVerts(tempVerts, tempIndices);
        mesh->UpdateVertices((unsigned int)tempVerts.size(), &tempVerts[0]);
        mesh->UpdateIndices((unsigned int)tempIndices.size(), &tempIndices[0]);
        ctx->BindDiffuseTexture(ob->GetTexture());
        ctx->DrawMesh(mesh);
    }

    ctx->EndCamera(debugCam);

    delete mesh;
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderEndFrame()
{
    sDebugRenderSystem->CleanDeadObjects();
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldPoint(Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DRObjectPoint* newPoint = new DRObjectPoint(pos, size, startColor, endColor, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newPoint);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldPoint(Vec3 pos, float size, Rgba8 color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWorldPoint(pos, size, color, color, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldPoint(Vec3 pos, Rgba8 color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWorldPoint(pos, sDebugRenderSystem->GetDefaultLineWidth()*.01f, color, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldLine(Vec3 p0, Rgba8 startColorP0, Rgba8 endColorP0, Vec3 p1, Rgba8 startColorP1, Rgba8 endColorP1, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DRObjectLine* newLine = new DRObjectLine(p0, startColorP0, endColorP0, p1, startColorP1, endColorP1, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newLine);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldLine(Vec3 p0, Vec3 p1, Rgba8 color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWorldLine(p0, color, color, p1, color, color, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldArrow(Vec3 p0, Rgba8 startColorP0, Rgba8 endColorP0, Vec3 p1, Rgba8 startColorP1, Rgba8 endColorP1, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DRObjectArrow* newArrow = new DRObjectArrow(p0, startColorP0, endColorP0, p1, startColorP1, endColorP1, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newArrow);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldArrow(Vec3 start, Vec3 end, Rgba8 color, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWorldArrow(start, color, color, end, color, color, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldQuad(Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DRObjectQuad* newQuad = new DRObjectQuad(p0, p1, p2, p3, startColor, endColor, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newQuad);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldWireBounds(OBB3 bounds, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    std::vector<Vertex_PCU> verts;
    std::vector<unsigned int> indices;
    AppendIndexedVertexesForAABB3D(verts, indices, AABB3( -bounds.halfDimensions, bounds.halfDimensions),startColor);
    Mat44 lookat = MatrixLookAt(Vec3::ZERO,bounds.GetForwardVector());
    lookat.SetBasisVectors3D(lookat.GetIBasis3D(),lookat.GetJBasis3D(),-lookat.GetKBasis3D(), bounds.center);
    TransformVertexArray((int)verts.size(), &verts[0], lookat);

    DRObjectWireBounds* newBounds = new DRObjectWireBounds(verts, indices, startColor, endColor, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject(newBounds);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldWireBounds(OBB3 bounds, Rgba8 color, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWorldWireBounds(bounds, color, color, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldWireBounds(AABB3 bounds, Rgba8 color, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    OBB3 obb(bounds);
    DebugAddWorldWireBounds(obb, color, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldWireSphere(Vec3 pos, float radius, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    std::vector<Vertex_PCU> verts;
    std::vector<unsigned int> indices;
    AppendIndexedVertexesForIcoSphere(verts, indices, pos, radius, 3, startColor);

    DRObjectWireBounds* newBounds = new DRObjectWireBounds(verts, indices, startColor, endColor, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newBounds);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldWireSphere(Vec3 pos, float radius, Rgba8 color, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWorldWireSphere(pos, radius, color, color, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldBasis(Mat44 basis, Rgba8 startTint, Rgba8 endTint, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DRObjectBasis* newBasis = new DRObjectBasis(basis, startTint, endTint, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject(newBasis);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldBasis(Mat44 basis, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWorldBasis(basis, Rgba8::WHITE, Rgba8::WHITE, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldText(Mat44 basis, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text)
{
    Vec3 pos = basis.GetTranslation3D();
    DRObjectText* newText = new DRObjectText(text, 1.f, startColor, endColor, false, pos, pivot,
        (double)duration, mode, true, Vec2::ZERO, basis);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldText(Mat44 basis, AABB2 bounds, Vec2 alignment, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text)
{
    DRObjectText* newText = new DRObjectText(text, 1.f, bounds, alignment, basis.GetTranslation3D(),
        startColor, endColor, false, duration, mode, true, basis);
    sDebugRenderSystem->AppendNewDebugRenderObject(newText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldTextf(Mat44 basis, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    DebugAddWorldText(basis, pivot, color, color, duration, mode, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldTextf(Mat44 basis, Vec2 pivot, Rgba8 color, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    DebugAddWorldText(basis, pivot, color, color, -1.f, eDebugRenderMode::DEBUG_RENDER_USE_DEPTH, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldBillboardText(Vec3 origin, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text)
{
    DRObjectText* newText = new DRObjectText(text, 1.f, startColor, endColor, true, origin,
        pivot, (double)duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldBillboardText(Vec3 origin, AABB2 bounds, Vec2 alignment, Rgba8 startColor, Rgba8 endColor, float duration, eDebugRenderMode mode, char const* text)
{
    DRObjectText* newText = new DRObjectText(text, 1.f, bounds, alignment, origin,
        startColor, endColor,true, duration, mode, true);
    sDebugRenderSystem->AppendNewDebugRenderObject(newText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldBillboardTextf(Vec3 origin, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    DebugAddWorldBillboardText(origin, pivot, color, color, duration, mode, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWorldBillboardTextf(Vec3 origin, Vec2 pivot, Rgba8 color, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    DebugAddWorldBillboardText(origin, pivot, color, color, -1.f, DEBUG_RENDER_USE_DEPTH, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWireMeshToWorld(Mat44 transform, GPUMesh* mesh, Rgba8 startTint, Rgba8 endTint, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DRObjectWireMesh* newMesh = new DRObjectWireMesh(transform, mesh, startTint, endTint, duration, mode);
    sDebugRenderSystem->AppendNewDebugRenderObject(newMesh);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddWireMeshToWorld(Mat44 transform, GPUMesh* mesh, Rgba8 tint /*white*/, float duration /*= 0.0f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
    DebugAddWireMeshToWorld(transform, mesh, tint, tint, duration, mode);
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderSetScreenHeight(float height)
{
    sDebugRenderSystem->SetScreenHeight(height);
}

//////////////////////////////////////////////////////////////////////////
AABB2 DebugGetScreenBounds()
{
    return sDebugRenderSystem->GetCamera()->GetBounds();
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenPoint(Vec2 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration)
{
    DRObjectPoint* newPoint = new DRObjectPoint(pos, size, startColor, endColor, (double)duration, DEBUG_RENDER_ALWAYS, false);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newPoint);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenPoint(Vec2 pos, float size, Rgba8 color, float duration /*= 0.0f*/)
{
    DebugAddScreenPoint(pos, size, color, color, duration);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenPoint(Vec2 pos, Rgba8 color)
{
    DebugAddScreenPoint(pos, sDebugRenderSystem->GetDefaultLineWidth(), color, -1.f);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenLine(Vec2 p0, Rgba8 startColorP0, Rgba8 endColorP0, Vec2 p1, Rgba8 startColorP1, Rgba8 endColorP1, float duration)
{
    DRObjectLine* newLine = new DRObjectLine(p0, startColorP0, endColorP0, p1, startColorP1, endColorP1, (double)duration, DEBUG_RENDER_ALWAYS, false);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newLine);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenLine(Vec2 p0, Vec2 p1, Rgba8 color, float duration /*= 0.0f*/)
{
    DebugAddScreenLine(p0, color, color, p1, color, color, duration);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenArrow(Vec2 p0, Rgba8 startColorP0, Rgba8 endColorP0, Vec2 p1, Rgba8 startColorP1, Rgba8 endColorP1, float duration)
{
    DRObjectArrow* newArrow = new DRObjectArrow(p0, startColorP0, endColorP0, p1, startColorP1, endColorP1, (double)duration, DEBUG_RENDER_ALWAYS, false);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newArrow);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenArrow(Vec2 p0, Vec2 p1, Rgba8 color, float duration /*= 0.0f*/)
{
    DebugAddScreenArrow(p0, color, color, p1, color, color, duration);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenQuad(AABB2 bounds, Rgba8 startColor, Rgba8 endColor, float duration)
{
    Vec2 mins = bounds.mins;
    Vec2 maxs = bounds.maxs;
    DRObjectQuad* newQuad = new DRObjectQuad(mins, Vec2(maxs.x, mins.y), maxs, Vec2(mins.x, maxs.y), startColor, endColor, (double)duration, DEBUG_RENDER_ALWAYS, false);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newQuad);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenQuad(AABB2 bounds, Rgba8 color, float duration /*= 0.0f*/)
{
    DebugAddScreenQuad(bounds, color, color, duration);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 startTint, Rgba8 endTint, float duration /*= 0.0f*/)
{
    DRObjectTexturedQuad* newTexQuad = new DRObjectTexturedQuad(bounds, tex, uvs, startTint, endTint, (double)duration, DEBUG_RENDER_ALWAYS);
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newTexQuad);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 tint, float duration /*= 0.0f*/)
{
    DebugAddScreenTexturedQuad(bounds, tex, uvs, tint, tint, duration);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenTexturedQuad(AABB2 bounds, Texture* tex, Rgba8 tint /*= Rgba8::WHITE*/, float duration /*= 0.0f*/)
{
    DebugAddScreenTexturedQuad(bounds, tex, AABB2(Vec2::ZERO, Vec2::ONE), tint, duration);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenText(Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, char const* text)
{
    DRObjectText* newText = new DRObjectText(text, size, startColor, endColor, false, Vec2(pos.x, pos.y),
        pivot, (double)duration, DEBUG_RENDER_ALWAYS, false, Vec2(pos.z, pos.w));
    sDebugRenderSystem->AppendNewDebugRenderObject((DebugRenderObject*)newText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenText(AABB2 bounds, Vec2 alignment, float size, Rgba8 startColor, Rgba8 endColor, float duration, char const* text)
{
    DRObjectText* newText = new DRObjectText(text, size, bounds, alignment, Vec3::ZERO, startColor, endColor, false, duration, DEBUG_RENDER_ALWAYS, false);
    sDebugRenderSystem->AppendNewDebugRenderObject(newText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    DebugAddScreenText(pos, pivot, size, startColor, endColor, duration, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 color, float duration, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    DebugAddScreenText(pos, pivot, size, color, color, duration, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, float size, Rgba8 color, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    DebugAddScreenText(pos, pivot, size, color, color, -1.f, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenTextf(Vec4 pos, Vec2 pivot, Rgba8 color, char const* format, ...)
{
    char trueText[STRING_STACK_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(trueText, STRING_STACK_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    trueText[STRING_STACK_LENGTH - 1] = '\0';
    float size = sDebugRenderSystem->GetDefaultLineWidth() * 5.f;
    DebugAddScreenText(pos, pivot, size, color, color, -1.f, trueText);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenBasis(Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 startTint, Rgba8 endTint, float duration)
{
    DRObjectScreenBasis* newBasis = new DRObjectScreenBasis(screen_origin_location, basis_to_render, startTint, endTint, duration);
    sDebugRenderSystem->AppendNewDebugRenderObject(newBasis);
}

//////////////////////////////////////////////////////////////////////////
void DebugAddScreenBasis(Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 tint /*= Rgba8::WHITE*/, float duration /*= 0.0f*/)
{
    DebugAddScreenBasis(screen_origin_location, basis_to_render, tint, tint, duration);
}

//------------------------------------------------------------------------
// Debug Render Console Controls
//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
COMMAND(debug_render, "control debug render on and off, enable=true", eEventFlag::EVENT_CONSOLE) 
{
    bool enabled = args.GetValue("0", false);

    if (enabled) {
        EnableDebugRendering();
    }
    else {
        ClearDebugRendering();
        DisableDebugRendering();
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(debug_add_world_point, "add world point, position=Vec3::ZERO, duration=-1.f", eEventFlag::EVENT_CONSOLE)
{
    Vec3 position = args.GetValue("position", Vec3::ZERO);
    float duration = args.GetValue("duration", -1.f);
    if (position == Vec3::ZERO && duration == -1.f) {
        position = args.GetValue("0", Vec3::ZERO);
        duration = args.GetValue("1", -1.f);
    }

    DebugAddWorldPoint(position, Rgba8::WHITE, duration);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(debug_add_world_wire_sphere, "add world wire sphere, position=Vec3::ZERO, radius=1.f, duration=-1.f", eEventFlag::EVENT_CONSOLE)
{
    Vec3 position = args.GetValue("position", Vec3::ZERO);
    float radius = args.GetValue("radius", 1.f);
    float duration = args.GetValue("duration", -1.f);
    if (position == Vec3::ZERO && radius == 1.f && duration == -1.f) {
        position = args.GetValue("0", Vec3::ZERO);
        radius = args.GetValue("1", 1.f);
        duration = args.GetValue("2", -1.f);
    }

    DebugAddWorldWireSphere(position, radius, Rgba8::WHITE, duration);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(debug_add_world_wire_bounds, "add world wire OBB3, min=Vec3(-1.f,-1.f,-1.f), max=Vec3(1.f,1.f,1.f), duration=-1.f", eEventFlag::EVENT_CONSOLE)
{
    Vec3 mins = args.GetValue("min", Vec3(-1.f));
    Vec3 maxs = args.GetValue("max", Vec3(1.f));
    float duration = args.GetValue("duration", -1.f);
    if (mins == Vec3(-1.f) && maxs == Vec3(1.f) && duration == -1.f) {
        mins = args.GetValue("0", Vec3(-1.f));
        maxs = args.GetValue("1", Vec3(1.f));
        duration = args.GetValue("2", -1.f);
    }

    DebugAddWorldWireBounds(AABB3(mins, maxs), Rgba8::WHITE, duration);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(debug_add_world_billboard_text, "add world billboard text, position=Vec3::ZERO, pivot=Vec2(.5f,.5f), text=\"Test\"", eEventFlag::EVENT_CONSOLE)
{
    Vec3 position = args.GetValue("position", Vec3::ZERO);
    Vec2 pivot = args.GetValue("pivot", Vec2(.5f, .5f));
    std::string text = args.GetValue("text", "Test");
    if(position==Vec3::ZERO && pivot==Vec2(.5f, .5f) && text=="Test"){
        position = args.GetValue("0", Vec3::ZERO);
        pivot = args.GetValue("1", Vec2(.5f, .5f));
        text = args.GetValue("2", "Test");
    }

    DebugAddWorldBillboardText(position, pivot, Rgba8::WHITE, Rgba8::WHITE, -1.f, DEBUG_RENDER_USE_DEPTH, text.c_str());
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(debug_add_screen_point, "add screen point, position=Vec2::ZERO, duration=-1.f", eEventFlag::EVENT_CONSOLE)
{
    Vec2 position = args.GetValue("position", Vec2::ZERO);
    float duration = args.GetValue("duration", -1.f);
    if (position == Vec2::ZERO && duration == -1.f) {
        position = args.GetValue("0", Vec2::ZERO);
        duration = args.GetValue("1", -1.f);
    }

    float size = sDebugRenderSystem->GetDefaultLineWidth();

    DebugAddScreenPoint(position, size, Rgba8::WHITE, duration);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(debug_add_screen_quad, "add screen quad, min=Vec2::ZERO, max=Vec2(100.f,100.f), duration=-1.f", eEventFlag::EVENT_CONSOLE)
{
    Vec2 mins = args.GetValue("min", Vec2::ZERO);
    Vec2 maxs = args.GetValue("max", Vec2(100.f, 100.f));
    float duration = args.GetValue("duration", -1.f);
    if (mins == Vec2::ZERO && maxs == Vec2(100.f, 100.f) && duration == -1.f) {
        mins = args.GetValue("0", Vec2::ZERO);
        maxs = args.GetValue("1", Vec2(100.f, 100.f));
        duration = args.GetValue("2", -1.f);
    }

    DebugAddScreenQuad(AABB2(mins, maxs), Rgba8::WHITE, duration);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(debug_add_screen_text, "add screen text, position=Vec2::ZERO, pivot=Vec2(.5f,.5f), text=\"Test\"", eEventFlag::EVENT_CONSOLE)
{
    Vec2 position = args.GetValue("position", Vec2::ZERO);
    Vec2 pivot = args.GetValue("pivot", Vec2(.5f, .5f));
    std::string text = args.GetValue("text", "Test");
    if (position == Vec2::ZERO && pivot == Vec2(.5f, .5f) && text == "Test") {
        position = args.GetValue("0", Vec2::ZERO);
        pivot = args.GetValue("1", Vec2(.5f, .5f));
        text = args.GetValue("2", "Test");
    }

    float size = sDebugRenderSystem->GetDefaultLineWidth() * 5.f;

    DebugAddScreenText(Vec4(0.f,0.f, position.x, position.y), pivot, size, Rgba8::WHITE, Rgba8::WHITE, -1.f, text.c_str());
    return true;
}