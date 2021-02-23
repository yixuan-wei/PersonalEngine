#pragma once

constexpr int CHILD_CLOCK_COUNT = 50;

class Clock
{
public:
    Clock();                    // defaults to being owned by master
    Clock(Clock* parent);       // passing nullptr will create a root clock (root clock is something you'd have to advance yourself)
    ~Clock();                  // be sure to correcty re-parent my children to my parent

    void Update(double deltaSeconds);     // usually do not need to call unless you create a new root clock
    void Reset();                       // set total time back to 0.0, does not reset children

    //Controls
    void Pause();
    void Resume();
    void SetScale(double scale);

    //Accessors
    double GetTotalElapsedSeconds() const   { return m_totalSeconds; }
    double GetLastDeltaSeconds() const      { return m_lastDeltaSeconds; }
    double GetScale() const                 { return m_timeScale; }
    bool   IsPaused() const                 { return m_isPaused; }

    void SetFrameLimits( double minFrameTime, double maxFrameTime ); // useful if you never want a simluation jumping too far (for exapmle, while stuck on a breakpoint)

public:
    //internally
    void SetParent(Clock* newParent);
    void AddChild(Clock* newChild);
    void RemoveChild(Clock* oldChild);

private:
    double m_timeScale = 1.0;
    double m_totalSeconds = 0.0;
    double m_lastDeltaSeconds = 0.0;

    double m_minFrameTime = 0.0;
    double m_maxFrameTime = 1000000;

    bool m_isPaused = false;

    Clock* m_parent = nullptr;
    Clock* m_children[CHILD_CLOCK_COUNT];

public:
    static void SystemStartup();
    static void SystemShutdown();
    static void BeginFrame();

    static Clock* GetMaster();
};