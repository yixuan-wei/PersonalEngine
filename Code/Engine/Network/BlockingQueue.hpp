#pragma once

#include <queue>
#include <mutex>

//////////////////////////////////////////////////////////////////////////
// Class Definitions
//////////////////////////////////////////////////////////////////////////
template<typename T>
class BlockingQueue : protected std::queue<T>
{
protected:
    using base = typename std::queue<T>;

public:
    BlockingQueue() : base() {}
    BlockingQueue(BlockingQueue const&) = delete;
    BlockingQueue(BlockingQueue&&) = delete; //move copy constructor: return temporary result
    ~BlockingQueue() = default;

    BlockingQueue& operator=(BlockingQueue const&) = delete;
    BlockingQueue& operator=(BlockingQueue&&) = delete;//move assignment operator

    void Push(T const& newElement);
    T Pop();

private:
    std::mutex m_lock;
    std::condition_variable m_condition;
};


//////////////////////////////////////////////////////////////////////////
// Function Definitions
//////////////////////////////////////////////////////////////////////////
template<typename T>
void BlockingQueue<T>::Push(T const& newElement)
{
    std::lock_guard<std::mutex> guard(m_lock);
    base::push(newElement);
    m_condition.notify_all();
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
T BlockingQueue<T>::Pop()
{
    T value = T();

    std::unique_lock<std::mutex> uniqueLock(m_lock);
    if (base::empty()) {
        m_condition.wait(uniqueLock);
    }
    value = base::front();
    base::pop();
    return value;
}