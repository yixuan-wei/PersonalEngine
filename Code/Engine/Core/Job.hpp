#pragma once

#include <atomic>

class Job;

enum eJobFlag : unsigned int
{
    JOB_DISK = (1 << 0),
    JOB_GENERAL = 0xffff
};

enum eJobStatus : int
{
    JOB_STAT_INIT = 0,
    JOB_STAT_QUEUING,
    JOB_STAT_RUNNING,
    JOB_STAT_COMPLETE,
    JOB_STAT_CALLBACK,
    JOB_STAT_FINISH
};

enum eJobPriority : int
{
    JOB_PRIO_FATAL  = -1,
    JOB_PRIO_HIGH   = 10,
    JOB_PRIO_MEDIUM = 100,
    JOB_PRIO_LOW    = 1000,
    JOB_PRIO_MAYBE  = 5000
};

void InitJobSystem(); 
void JobSystemBeginFrame();
void CloseJobSystem();

void CreateWorkerThreads(unsigned int number, unsigned int jobFlags = eJobFlag::JOB_GENERAL);
void PostJob(Job& job);
void ClaimAndDeleteAllCompletedJobs();
void WaitForJob(int jobID);
void WaitForNextJobOfType(unsigned int jobFlags);
void WaitForAllJobsOfType(unsigned int jobFlags);

unsigned int GetHardwareConcurrency();

//////////////////////////////////////////////////////////////////////////
class Job
{
public:
    Job(unsigned int jobFlags, int priority);
    virtual ~Job() {}

    virtual void Execute() = 0;
    virtual void OnCompleteCallback() {} //Not every job needs a callback

    void MakeUrgent();
    void UpdatePriority();
    void ProgressStatus();

    unsigned int GetJobFlags() const {return m_jobFlags;}
    int GetPriority() const {return m_priority;}
    int GetJobID() const {return m_jobID;}
    eJobStatus GetJobStatus() const;

private:
    std::atomic<int> m_jobStatus = 0;
    std::atomic<int> m_jobID = 0;
    const unsigned int m_jobFlags = 0;
    std::atomic<int> m_priority = 1;
};