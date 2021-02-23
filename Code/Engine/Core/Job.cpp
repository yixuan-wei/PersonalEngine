#include "Engine/Core/Job.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <thread>
#include <mutex>
#include <deque>

//////////////////////////////////////////////////////////////////////////
// definitions
//////////////////////////////////////////////////////////////////////////
class JobSystem;

static JobSystem* sJobSystem = nullptr;

//////////////////////////////////////////////////////////////////////////
class JobSystemWorkerThread
{
public:
    void WorkerThreadMain();

    JobSystemWorkerThread(unsigned int jobFlags);
    ~JobSystemWorkerThread();

private:
    std::thread* m_threadObject = nullptr;
    unsigned int m_jobFlags=0;
    int m_threadID = 0;
};

//////////////////////////////////////////////////////////////////////////
class JobSystem
{
public:
    JobSystem();
    ~JobSystem();

    void UpdatePrioritiesForQueuedJobs();

    void CreateWorkerThread(unsigned int jobFlags);
    void PostJob(Job* job);
    void ClaimAndDeleteAllCompletedJobs();

    Job* FetchOneJob(unsigned int jobFlags);
    void ReturnCompleteJob(Job* job);

    void FinishJob(int jobID);
    void FinishAnyJobOfType(unsigned int jobFlags);
    void FinishAllJobsOfType(unsigned int jobFlags);

    Job* GetJob(int jobID) const;
    Job* GetJobOfType(unsigned int jobFlags) const;
    void GetAllJobsOfType(std::vector<Job*>& allJobs, unsigned int jobFlags) const;
    bool IsQuiting() const {return m_isQuiting;}
    bool IsJobComplete(int jobID) const;

private:
    void MakeJobUrgent(int jobID);  //assume job is in queuing
    void DoJobCallback(int jobID);  //assume job is complete

private:
    std::atomic<bool> m_isQuiting = false;

    std::deque<Job*> m_jobsQueued;
    std::deque<Job*> m_jobsRunning;
    std::deque<Job*> m_jobsCompleted;
    mutable std::mutex m_jobsQueuedMutex;
    mutable std::mutex m_jobsRunningMutex;
    mutable std::mutex m_jobsCompleteMutex;

    std::vector<JobSystemWorkerThread*> m_workerThreads;
};


//////////////////////////////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////////////////////////////
void InitJobSystem()
{
    sJobSystem = new JobSystem();
}

//////////////////////////////////////////////////////////////////////////
void CreateWorkerThreads(unsigned int number, unsigned int jobFlags)
{
    if (sJobSystem == nullptr) {
        g_theConsole->PrintError("Job System not exist while create worker threads");
        return;
    }

    for (unsigned int i = 0; i < number; i++) {
        sJobSystem->CreateWorkerThread(jobFlags);
    }
}

//////////////////////////////////////////////////////////////////////////
void JobSystemBeginFrame()
{
    if (sJobSystem==nullptr || sJobSystem->IsQuiting()) {
        return;
    }

    sJobSystem->UpdatePrioritiesForQueuedJobs();
}

//////////////////////////////////////////////////////////////////////////
void PostJob(Job& job)
{
    if (sJobSystem == nullptr) {
        g_theConsole->PrintError("Job System not exist while post job");
        return;
    }

    sJobSystem->PostJob(&job);
}

//////////////////////////////////////////////////////////////////////////
void ClaimAndDeleteAllCompletedJobs()
{
    if (sJobSystem == nullptr) {
        g_theConsole->PrintError("Job System not exist while claim all complete jobs");
        return;
    }

    sJobSystem->ClaimAndDeleteAllCompletedJobs();
}

//////////////////////////////////////////////////////////////////////////
void WaitForJob(int jobID)
{
    if (sJobSystem == nullptr) {
        g_theConsole->PrintError("Job System not exist while wait for job");
        return;
    }

    sJobSystem->FinishJob(jobID);
}

//////////////////////////////////////////////////////////////////////////
void WaitForNextJobOfType(unsigned int jobFlags)
{
    if (sJobSystem == nullptr) {
        g_theConsole->PrintError("Job System not exist while wait for next job of type");
        return;
    }

    sJobSystem->FinishAnyJobOfType(jobFlags);
}

//////////////////////////////////////////////////////////////////////////
void WaitForAllJobsOfType(unsigned int jobFlags)
{
    if (sJobSystem == nullptr) {
        g_theConsole->PrintError("Job System not exist while wait for all job of type");
        return;
    }

    sJobSystem->FinishAllJobsOfType(jobFlags);
}

//////////////////////////////////////////////////////////////////////////
void CloseJobSystem()
{
    if(sJobSystem!=nullptr){
        delete sJobSystem;
        sJobSystem = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////
unsigned int GetHardwareConcurrency()
{
    return std::thread::hardware_concurrency();
}

//////////////////////////////////////////////////////////////////////////
Job::Job(unsigned int jobFlags, int priority)
    : m_jobFlags(jobFlags)
    , m_priority(priority)
{
    static int s_nextJobID = 0;
    m_jobID = s_nextJobID++;
}

//////////////////////////////////////////////////////////////////////////
void Job::MakeUrgent()
{
    m_priority=-1;
}

//////////////////////////////////////////////////////////////////////////
void Job::UpdatePriority()
{
    m_priority--;
}

//////////////////////////////////////////////////////////////////////////
void Job::ProgressStatus()
{
    m_jobStatus++;
}

//////////////////////////////////////////////////////////////////////////
eJobStatus Job::GetJobStatus() const
{
    return (eJobStatus)(int)m_jobStatus;
}

//////////////////////////////////////////////////////////////////////////
void JobSystemWorkerThread::WorkerThreadMain()
{    
    Rgba8 yellow(255,255,0);
    std::string printText = Stringf("worker thread #%i...", m_threadID);
    g_theConsole->PrintString(yellow, "Start " + printText);

    while (!sJobSystem->IsQuiting()) {
        Job* newJob = sJobSystem->FetchOneJob(m_jobFlags);
        if (newJob != nullptr) {
            newJob->Execute();  
            sJobSystem->ReturnCompleteJob(newJob);

            g_theConsole->PrintString(Rgba8(255,255,255), "Job "+std::to_string(newJob->GetJobID())+" done by %s"+printText);
        }
        else {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }

    g_theConsole->PrintString(yellow, "End "+printText);
}

//////////////////////////////////////////////////////////////////////////
JobSystemWorkerThread::JobSystemWorkerThread(unsigned int jobFlags)
    : m_jobFlags(jobFlags)
{
    m_threadObject = new std::thread(&JobSystemWorkerThread::WorkerThreadMain, this);
    static int s_nextThreadID = 0;
    m_threadID = s_nextThreadID++;
}

//////////////////////////////////////////////////////////////////////////
JobSystemWorkerThread::~JobSystemWorkerThread()
{
    m_threadObject->join();
    delete m_threadObject;
    m_threadObject = nullptr;
}

//////////////////////////////////////////////////////////////////////////
JobSystem::JobSystem()
{
    if (sJobSystem != nullptr) {
        ERROR_AND_DIE("Multiple Job system existed at same time");
    }
}

//////////////////////////////////////////////////////////////////////////
JobSystem::~JobSystem()
{
    m_isQuiting = true;
    for (size_t i = 0; i < m_workerThreads.size(); i++) {
        delete m_workerThreads[i];
        m_workerThreads[i] = nullptr;
    }
    ClaimAndDeleteAllCompletedJobs();
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::UpdatePrioritiesForQueuedJobs()
{
    m_jobsQueuedMutex.lock();
    for (Job* job : m_jobsQueued)    {
        job->UpdatePriority();
    }
    m_jobsQueuedMutex.unlock();
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::CreateWorkerThread(unsigned int jobFlags)
{
    JobSystemWorkerThread* newThread = new JobSystemWorkerThread(jobFlags);
    m_workerThreads.push_back(newThread);
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::PostJob(Job* job)
{
    m_jobsQueuedMutex.lock();
    for (std::deque<Job*>::iterator it = m_jobsQueued.begin(); it != m_jobsQueued.end(); ++it) {
        if (job->GetPriority() < (*it)->GetPriority()) {
            m_jobsQueued.insert(it,job);
            m_jobsQueuedMutex.unlock();
            job->ProgressStatus();  //init -> queuing
            return;
        }
    }

    m_jobsQueued.push_back(job);
    m_jobsQueuedMutex.unlock();
    job->ProgressStatus();//init -> queuing
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::ClaimAndDeleteAllCompletedJobs()
{
    std::deque<Job*> completeJobs;
    m_jobsCompleteMutex.lock();
    m_jobsCompleted.swap(completeJobs);
    m_jobsCompleteMutex.unlock();

    for (Job* job : completeJobs) {
        job->ProgressStatus();  //complete->callback
        job->OnCompleteCallback();
        job->ProgressStatus();  //callback -> finish
        delete job;
    }
}

//////////////////////////////////////////////////////////////////////////
Job* JobSystem::FetchOneJob(unsigned int jobFlags)
{
    Job* result = nullptr;
    {
        std::scoped_lock lock(m_jobsQueuedMutex, m_jobsRunningMutex);
        if (!m_jobsQueued.empty()) {
            for (std::deque<Job*>::iterator it = m_jobsQueued.begin(); it != m_jobsQueued.end(); ++it) {
                Job* job = *it;
                if (job->GetJobFlags() & jobFlags) {
                    result = job;
                    m_jobsQueued.erase(it);
                    m_jobsRunning.push_back(result);
                    job->ProgressStatus();  //queuing -> running
                    break;
                }
            }            
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::ReturnCompleteJob(Job* job)
{
    std::scoped_lock lock(m_jobsRunningMutex, m_jobsCompleteMutex);
    m_jobsCompleted.push_back(job);
    for (std::deque<Job*>::iterator it = m_jobsRunning.begin(); it != m_jobsRunning.end(); ++it) {
        if (*it == job) {
            m_jobsRunning.erase(it);
            job->ProgressStatus(); //running -> complete
            return;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::FinishJob(int jobID)
{
    Job* thisJob = nullptr;
    thisJob = GetJob(jobID);
    if (thisJob == nullptr) {
        g_theConsole->PrintString(Rgba8::MAGENTA,Stringf("job of id %i doesn't exist in job system", jobID));
        return;
    }

    if (thisJob->GetJobStatus() == JOB_STAT_QUEUING && thisJob->GetPriority()>=0) {
        MakeJobUrgent(jobID);
    }

    while(thisJob->GetJobStatus()!=JOB_STAT_COMPLETE){
        std::this_thread::yield();
    } //wait for complete

    DoJobCallback(jobID);
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::FinishAnyJobOfType(unsigned int jobFlags)
{
    Job* aJob = nullptr;
    aJob = GetJobOfType(jobFlags);
    if (aJob == nullptr) {
        g_theConsole->PrintString(Rgba8::MAGENTA,Stringf("job of flags %u no exist in job system",jobFlags));
        return;
    }

    if (aJob->GetJobStatus() == JOB_STAT_QUEUING && aJob->GetPriority()>=0) {
        MakeJobUrgent(aJob->GetJobID());
    }

    while (aJob->GetJobStatus() != JOB_STAT_COMPLETE) {
        std::this_thread::yield();
    }

    DoJobCallback(aJob->GetJobID());
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::FinishAllJobsOfType(unsigned int jobFlags)
{
    std::vector<Job*> allJobs;
    GetAllJobsOfType(allJobs, jobFlags);

    //make queuing jobs urgent
    for (Job* job : allJobs) {
        if (job->GetJobStatus() == JOB_STAT_QUEUING && job->GetPriority()>=0) {
            MakeJobUrgent(job->GetJobID());
        }
    }

    //wait for jobs to complete
    bool isComplete = false;
    while (!isComplete) {
        isComplete=true;
        for (Job* job : allJobs) {
            if(job->GetJobStatus()!=JOB_STAT_FINISH){
                isComplete=false;
                if (job->GetJobStatus() == JOB_STAT_COMPLETE) {
                    DoJobCallback(job->GetJobID());
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
Job* JobSystem::GetJob(int jobID) const
{
    //complete
    m_jobsCompleteMutex.lock();
    for (Job* job : m_jobsCompleted) {
        if (job->GetJobID() == jobID) {
            m_jobsCompleteMutex.unlock();
            return job;
        }
    }
    m_jobsCompleteMutex.unlock();

    //running
    m_jobsRunningMutex.lock();
    for (Job* job : m_jobsRunning) {
        if (job->GetJobID() == jobID) {
            m_jobsRunningMutex.unlock();
            return job;
        }
    }
    m_jobsRunningMutex.unlock();    

    //queued
    m_jobsQueuedMutex.lock();
    for (Job* job : m_jobsQueued) {
        if (job->GetJobID() == jobID) {
            m_jobsQueuedMutex.unlock();
            return job;
        }
    }
    m_jobsQueuedMutex.unlock();

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
Job* JobSystem::GetJobOfType(unsigned int jobFlags) const
{
    //complete
    m_jobsCompleteMutex.lock();
    for (Job* job : m_jobsCompleted) {
        if (job->GetJobFlags() & jobFlags) {
            m_jobsCompleteMutex.unlock();
            return job;
        }
    }
    m_jobsCompleteMutex.unlock();

     //running
    m_jobsRunningMutex.lock();
    for (Job* job : m_jobsRunning) {
        if (job->GetJobFlags() & jobFlags) {
            m_jobsRunningMutex.unlock();
            return job;
        }
    }
    m_jobsRunningMutex.unlock();

    //queued
    m_jobsQueuedMutex.lock();
    for (Job* job : m_jobsQueued) {
        if (job->GetJobFlags() & jobFlags) {
            m_jobsQueuedMutex.unlock();
            return job;
        }
    }
    m_jobsQueuedMutex.unlock();

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::GetAllJobsOfType(std::vector<Job*>& allJobs, unsigned int jobFlags) const
{
    m_jobsCompleteMutex.lock();
    for (Job* job : m_jobsCompleted) {
        if (job->GetJobFlags() & jobFlags) {
            allJobs.push_back(job);
        }
    }
    m_jobsCompleteMutex.unlock();

     //running
    m_jobsRunningMutex.lock();
    for (Job* job : m_jobsRunning) {
        if (job->GetJobFlags() & jobFlags) {
            allJobs.push_back(job);
        }
    }
    m_jobsRunningMutex.unlock();

    //queued
    m_jobsQueuedMutex.lock();
    for (Job* job : m_jobsQueued) {
        if (job->GetJobFlags() & jobFlags) {
            allJobs.push_back(job);
        }
    }
    m_jobsQueuedMutex.unlock();
}

//////////////////////////////////////////////////////////////////////////
bool JobSystem::IsJobComplete(int jobID) const
{
    m_jobsCompleteMutex.lock();
    for (Job* job : m_jobsCompleted) {
        if (job->GetJobID() == jobID) {
            m_jobsCompleteMutex.unlock();
            return true;
        }
    }
    m_jobsCompleteMutex.unlock();
    return false;
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::MakeJobUrgent(int jobID)
{
    Job* job = nullptr;

    m_jobsQueuedMutex.lock();
    for (std::deque<Job*>::iterator it = m_jobsQueued.begin(); it!=m_jobsQueued.end();++it) {
        job = *it;
        if (job->GetJobID() == jobID) {
            m_jobsQueued.erase(it);
            break;
        }
    }
    if (job != nullptr) {
        m_jobsQueued.push_front(job);
    }
    m_jobsQueuedMutex.unlock();

    if (job != nullptr) {
        job->MakeUrgent();
    }
}

//////////////////////////////////////////////////////////////////////////
void JobSystem::DoJobCallback(int jobID)
{
    Job* job=nullptr;

    m_jobsCompleteMutex.lock();
    for (std::deque<Job*>::iterator it = m_jobsCompleted.begin(); it != m_jobsCompleted.end(); ++it) {
        job=*it;
        if (job->GetJobID() == jobID) {
            m_jobsCompleted.erase(it);
            break;
        }
    }
    m_jobsCompleteMutex.unlock();

    if (job != nullptr) {
        job->ProgressStatus();  //complete->callback
        job->OnCompleteCallback();
        job->ProgressStatus();  //callback -> finish
        delete job;
    }
}
