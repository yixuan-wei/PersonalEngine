#pragma once

#include <queue>
#include <atomic>

//////////////////////////////////////////////////////////////////////////
// class definitions
//////////////////////////////////////////////////////////////////////////
template <typename T>
class ParallelQueue : protected std::queue<T>
{
protected:
    using base = std::queue<T>;

public:
    ParallelQueue() : base() {}
    ParallelQueue(ParallelQueue const&) = delete;
    ParallelQueue(ParallelQueue&&) = delete;
    ~ParallelQueue() = default;

    ParallelQueue& operator=(ParallelQueue const&) = delete;
    ParallelQueue& operator=(ParallelQueue&&) = delete;

    void Push(T const& value);
    T Pop();

protected:
    void Lock();
    void Unlock();

private:
    std::atomic<int> m_atomicLock;
    const int _UNLOCKED = 0;
    const int _LOCKED = 1;
};


//////////////////////////////////////////////////////////////////////////
// functions definitions
//////////////////////////////////////////////////////////////////////////
template <typename T>
void ParallelQueue<T>::Push(T const& value)
{
    Lock();
    base::push(value);
    Unlock();
}

//////////////////////////////////////////////////////////////////////////
template <typename T>
T ParallelQueue<T>::Pop()
{
    T value = T(); //not suitable for string

    Lock();
    if (!base::empty()) {
        value = base::front();
        base::pop();
    }    
    Unlock();
    return value;
}

//////////////////////////////////////////////////////////////////////////
template <typename T>
void ParallelQueue<T>::Lock()
{
    int expected = _UNLOCKED;
    while (!m_atomicLock.compare_exchange_strong(expected, _LOCKED));
}

//////////////////////////////////////////////////////////////////////////
template <typename T>
void ParallelQueue<T>::Unlock()
{
    int expected = _LOCKED;
    while(!m_atomicLock.compare_exchange_strong(expected, _UNLOCKED));
}