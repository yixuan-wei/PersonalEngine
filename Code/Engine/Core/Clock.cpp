#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

static Clock gMasterClock(nullptr);

//////////////////////////////////////////////////////////////////////////
Clock::Clock(Clock* parent)
    :m_parent(parent)
{
    for (int idx = 0; idx < CHILD_CLOCK_COUNT; idx++) {
        m_children[idx] = nullptr;
    }
    if (m_parent != nullptr) {
        m_parent->AddChild(this);
    }
}

//////////////////////////////////////////////////////////////////////////
Clock::Clock()
    :m_parent(&gMasterClock)
{
    for (int idx = 0; idx < CHILD_CLOCK_COUNT; idx++) {
        m_children[idx] = nullptr;
    }
    gMasterClock.AddChild(this);
}

//////////////////////////////////////////////////////////////////////////
Clock::~Clock()
{
    for (int idx = 0; idx < CHILD_CLOCK_COUNT; idx++) {
        Clock* child = m_children[idx];
        if (child != nullptr) {
            child->SetParent(m_parent);
        }
    }
    if(m_parent!=nullptr){
        m_parent->RemoveChild(this);
    }
}

//////////////////////////////////////////////////////////////////////////
void Clock::Update(double deltaSeconds)
{
    if (m_isPaused) {
        deltaSeconds = 0.f;
    }

    deltaSeconds *= m_timeScale;
    deltaSeconds = Clamp(deltaSeconds, m_minFrameTime, m_maxFrameTime);

    m_lastDeltaSeconds = deltaSeconds;
    m_totalSeconds += deltaSeconds;

    for (int idx = 0; idx < CHILD_CLOCK_COUNT; idx++) {
        Clock* child = m_children[idx];
        if (child != nullptr) {
            child->Update(deltaSeconds);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Clock::Reset()
{
    m_totalSeconds = 0.0;
}

//////////////////////////////////////////////////////////////////////////
void Clock::Pause()
{
    m_isPaused = true;
}

//////////////////////////////////////////////////////////////////////////
void Clock::Resume()
{
    m_isPaused = false;
}

//////////////////////////////////////////////////////////////////////////
void Clock::SetScale(double scale)
{
    m_timeScale = scale;
}

//////////////////////////////////////////////////////////////////////////
void Clock::SetFrameLimits(double minFrameTime, double maxFrameTime)
{
    m_minFrameTime = minFrameTime;
    m_maxFrameTime = maxFrameTime;
}

//////////////////////////////////////////////////////////////////////////
void Clock::SetParent(Clock* newParent)
{
    if (m_parent != nullptr) {
        m_parent->RemoveChild(this);
    }

    m_parent = newParent;
    if (m_parent != nullptr) {
        m_parent->AddChild(this);
    }
}

//////////////////////////////////////////////////////////////////////////
void Clock::AddChild(Clock* newChild)
{
    for (int idx = 0; idx < CHILD_CLOCK_COUNT; idx++) {
        Clock* child = m_children[idx];
        if (child == nullptr) {
            m_children[idx] = newChild;
            return;
        }
    }

    ERROR_AND_DIE("Can't add more child clock");
}

//////////////////////////////////////////////////////////////////////////
void Clock::RemoveChild(Clock* oldChild)
{
    for (int idx = 0; idx < CHILD_CLOCK_COUNT; idx++) {
        if (m_children[idx] == oldChild) {
            m_children[idx] = nullptr;
            return;
        }
    }

    ERROR_RECOVERABLE("Fail to delte a child clock");
}

//////////////////////////////////////////////////////////////////////////
void Clock::SystemStartup()
{
    gMasterClock.Reset();
}

//////////////////////////////////////////////////////////////////////////
void Clock::SystemShutdown()
{
    gMasterClock.Reset();
}

//////////////////////////////////////////////////////////////////////////
void Clock::BeginFrame()
{
    static double timePreviousFrame = GetCurrentTimeSeconds();
    double timeThisFrame = GetCurrentTimeSeconds();

    double deltaSeconds = timeThisFrame - timePreviousFrame;
    timePreviousFrame = timeThisFrame;

    gMasterClock.Update(deltaSeconds);
}

//////////////////////////////////////////////////////////////////////////
Clock* Clock::GetMaster()
{
    return &gMasterClock;
}
