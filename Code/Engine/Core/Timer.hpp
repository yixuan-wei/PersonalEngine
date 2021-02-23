#pragma once

class Clock;

class Timer
{
public:
    void SetTimerSeconds(Clock* c, double timeToWait);
    void SetTimerSeconds(double timeToWait);

    void Reset();
    void Stop();

    double GetElapsedSeconds() const;
    double GetRemainingSeconds() const;

    bool IsRunning() const;
    bool HasElapsed() const;
    bool CheckAndDecrement();
    int  CheckAndDecrementAll();
    bool CheckAndReset();

    // Pause and Resume require some extra bookkeeping, but can be useful
      // void Pause(); 
      // void Resume(); 

public:
    Clock* m_clock = nullptr;

    double m_startSeconds = 0.0;
    double m_durationSeconds = -1.0;    //negative means stopped
};