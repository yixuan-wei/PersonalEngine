#pragma once

#include <functional>

//////////////////////////////////////////////////////////////////////////
//Class Definitions
//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
class Delegate
{
public:
    using DelegateCallbackFunction = std::function<void(ARGS...)>;
    using DelegateCallback = void(*)(ARGS...);

    struct Subscription
    {
        void const* obj_id = nullptr;
        void const* func_id = nullptr;
        DelegateCallbackFunction callable;

        inline bool operator==(Subscription const& sub) const 
        { return (obj_id==sub.obj_id)&&(func_id == sub.func_id); }
    };

public:
    template <typename OBJ_TYPE>
    void SubscribeMethod(OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...));
    template <typename OBJ_TYPE>
    void UnsubscribeMethod(OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...));  
    template<typename OBJ_TYPE>
    void UnsubscribeObject(OBJ_TYPE* obj);

    void Subscribe(DelegateCallback const& cb);
    void Unsubscribe(DelegateCallback const& cb);    
    void Invoke(ARGS const& ...args);
    void operator() (ARGS const& ...args) { Invoke(args...); }

private:
    void Subscribe(Subscription const& s);
    void Unsubscribe(Subscription const& sub);    

    std::vector <Subscription> m_callbacks;
};


//////////////////////////////////////////////////////////////////////////
// Function Definitions
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template <typename ...ARGS>   // template for the class
template <typename OBJ_TYPE>  // template for the method
void Delegate<ARGS...>::SubscribeMethod(OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...))
{
    Subscription sub;
    sub.obj_id = obj;
    sub.func_id = *(void const**)&mcb;
    //std::bind
    sub.callable = [=](ARGS ...args) {(obj->*mcb)(args...); };
    Subscribe(sub);
}

//////////////////////////////////////////////////////////////////////////
template <typename ...ARGS>   // template for the class
template <typename OBJ_TYPE>  // template for the method
void Delegate<ARGS...>::UnsubscribeMethod(OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...))
{
    Subscription sub;
    sub.obj_id = obj;
    sub.func_id = *(void const**)&mcb;
    Unsubscribe(sub);
}

//////////////////////////////////////////////////////////////////////////
template <typename ...ARGS>
template<typename OBJ_TYPE>
void Delegate<ARGS...>::UnsubscribeObject(OBJ_TYPE* obj)
{
    for (size_t i = 0; i < m_callbacks.size();) {
        if (m_callbacks[i].obj_id == obj) {
            m_callbacks.erase(m_callbacks.begin() + i);
        }
        else {
            i++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
void Delegate<ARGS...>::Invoke(ARGS const& ...args)
{
    for (Subscription& sub : m_callbacks) {
        sub.callable(args...);
    }
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
void Delegate<ARGS...>::Unsubscribe(Subscription const& sub)
{
    for (size_t i = 0; i < m_callbacks.size(); i++) {
        if (m_callbacks[i] == sub) {
            m_callbacks.erase(m_callbacks.begin() + i);
            return;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
void Delegate<ARGS...>::Subscribe(Subscription const& s)
{
    m_callbacks.push_back(s);
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
void Delegate<ARGS...>::Subscribe(DelegateCallback const& cb)
{
    Subscription sub;
    sub.func_id = cb;
    sub.callable = cb;
    Subscribe(sub);
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
void Delegate<ARGS...>::Unsubscribe(DelegateCallback const& cb)
{
    Subscription sub;
    sub.func_id = cb;
    Unsubscribe(sub);
}

