#pragma once

#include <functional>

//////////////////////////////////////////////////////////////////////////
// Class Definition
//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
class ConsumedDelegate
{
public:
    using DelegateCallbackFunction = std::function<bool(ARGS...)>;
    using DelegateCallback = bool(*)(ARGS...);

    struct Subscription
    {
        void const* obj_id = nullptr;
        void const* func_id = nullptr;
        DelegateCallbackFunction callable;

        inline bool operator==(Subscription const& sub) const {
            return (obj_id == sub.obj_id) && (func_id == sub.func_id);
        }
    };

public:
    template<typename OBJ_TYPE>
    void SubscribeMethod(OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(ARGS...));
    template<typename OBJ_TYPE>
    void UnsubscribeMethod(OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(ARGS...));
    template<typename OBJ_TYPE>
    void UnsubscribeObject(OBJ_TYPE* obj);

    void Subscribe(DelegateCallback const& cb);
    void Unsubscribe(DelegateCallback const& cb);
    bool Invoke(ARGS const& ...args);
    bool operator()(ARGS const& ...args) { return Invoke(args...); }

    size_t GetCallbackCount() const { return m_callbacks.size(); }

private:
    void Subscribe(Subscription const& s);
    void Unsubscribe(Subscription const& s);

    std::vector<Subscription> m_callbacks;
};


//////////////////////////////////////////////////////////////////////////
// class definitions
//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
template<typename OBJ_TYPE>
void ConsumedDelegate<ARGS...>::SubscribeMethod(OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(ARGS...))
{
    Subscription sub;
    sub.obj_id = obj;
    sub.func_id = *(void const**)&mcb;
    sub.callable = [=](ARGS ...args)->bool {return (obj->*mcb)(args...); };
    Subscribe(sub);
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
template<typename OBJ_TYPE>
void ConsumedDelegate<ARGS...>::UnsubscribeMethod(OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(ARGS...))
{
    Subscription sub;
    sub.obj_id = obj;
    sub.func_id = *(void const**)&mcb;
    Unsubscribe(sub);
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
template<typename OBJ_TYPE>
void ConsumedDelegate<ARGS...>::UnsubscribeObject(OBJ_TYPE* obj)
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
void ConsumedDelegate<ARGS...>::Subscribe(DelegateCallback const& cb)
{
    Subscription sub;
    sub.func_id = cb;
    sub.callable = cb;
    Subscribe(sub);
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
void ConsumedDelegate<ARGS...>::Unsubscribe(DelegateCallback const& cb)
{
    Subscription sub;
    sub.func_id = cb;
    Unsubscribe(sub);
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
bool ConsumedDelegate<ARGS...>::Invoke(ARGS const& ...args)
{
    for (Subscription& sub : m_callbacks) {
        if (sub.callable(args...)) {
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
template<typename ...ARGS>
void ConsumedDelegate<ARGS...>::Subscribe(Subscription const& s)
{
    m_callbacks.push_back(s);
}

//////////////////////////////////////////////////////////////////////////

template<typename ...ARGS>
void ConsumedDelegate<ARGS...>::Unsubscribe(Subscription const& s)
{
    for (size_t i = 0; i < m_callbacks.size(); i++) {
        if (m_callbacks[i] == s) {
            m_callbacks.erase(m_callbacks.begin() + i);
            return;
        }
    }
}