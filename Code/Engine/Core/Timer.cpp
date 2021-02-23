#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

//////////////////////////////////////////////////////////////////////////
void Timer::SetTimerSeconds(Clock* c, double timeToWait)
{
    m_clock = c;
    m_startSeconds = m_clock->GetTotalElapsedSeconds();
    m_durationSeconds = timeToWait;
}

//////////////////////////////////////////////////////////////////////////
void Timer::SetTimerSeconds(double timeToWait)
{
    if (m_clock == nullptr) {
        m_clock = Clock::GetMaster();
    }
    m_startSeconds = m_clock->GetTotalElapsedSeconds();
    m_durationSeconds = timeToWait;
}

//////////////////////////////////////////////////////////////////////////
void Timer::Reset()
{
    m_startSeconds = m_clock->GetTotalElapsedSeconds();
}

//////////////////////////////////////////////////////////////////////////
void Timer::Stop()
{
    m_durationSeconds = -1.0;
}

//////////////////////////////////////////////////////////////////////////
double Timer::GetElapsedSeconds() const
{
    return m_clock->GetTotalElapsedSeconds() - m_startSeconds;
}

//////////////////////////////////////////////////////////////////////////
double Timer::GetRemainingSeconds() const
{
    return m_startSeconds + m_durationSeconds - m_clock->GetTotalElapsedSeconds();
}

//////////////////////////////////////////////////////////////////////////
bool Timer::IsRunning() const
{
    return m_durationSeconds >= 0.0;
}

//////////////////////////////////////////////////////////////////////////
bool Timer::HasElapsed() const
{
    return (m_clock->GetTotalElapsedSeconds() >= (m_startSeconds + m_durationSeconds));
}

//////////////////////////////////////////////////////////////////////////
bool Timer::CheckAndDecrement()
{
    if (HasElapsed()) {
        m_startSeconds += m_durationSeconds;
        return true;
    }
    else {
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////
int Timer::CheckAndDecrementAll()
{
    int runTimes = 0;
    while (HasElapsed()) {
        m_startSeconds += m_durationSeconds;
        runTimes++;
    }
    return runTimes;
}

//////////////////////////////////////////////////////////////////////////
bool Timer::CheckAndReset()
{
    if (HasElapsed()) {
        Reset();
        return true;
    }
    else {
        return false;
    }
}
