#pragma once

#include <string>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ConsumedDelegate.hpp"

class NamedProperties;

typedef NamedProperties EventArgs;
typedef bool (*EventCallbackFunction)(EventArgs& args);
#define COMMAND(name, desc, flags)\
		static bool name##_impl(NamedProperties& args);\
		static EventSubscription name##_register(#name, name##_impl, desc, flags);\
		static bool name##_impl(NamedProperties& args)

enum eEventFlag : unsigned int
{
	EVENT_CONSOLE = (1<<0),
	EVENT_PHYSICS = (1<<1),
	EVENT_GAME = (1<<2),
	EVENT_NETWORK = (1<<3),
	EVENT_GLOBAL = 0xffff
};

//////////////////////////////////////////////////////////////////////////
struct EventSubscription
{
	char const* eventName;
	ConsumedDelegate<EventArgs&> functionPtr;
	char const* description;
	unsigned int flags = 0;

	EventSubscription(char const* name, EventCallbackFunction fPtr, char const* desc, unsigned int flags);
    template<typename OBJ_TYPE>
    EventSubscription(char const* eventName, OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(EventArgs&), char const* description, unsigned int flags);
};

//////////////////////////////////////////////////////////////////////////
class EventSystem
{
public:
	EventSystem();

	bool FireEvent( std::string const& eventRawString, unsigned int flags = eEventFlag::EVENT_GLOBAL );
	bool FireEvent(std::string const& eventName, NamedProperties& parameters, unsigned int flags=eEventFlag::EVENT_GLOBAL);

	void RegisterEvent( std::string const& eventName, EventCallbackFunction functionPtr, std::string const& description="", unsigned int flags = eEventFlag::EVENT_GLOBAL);
	template<typename OBJ_TYPE>
	void RegisterMethodEvent(char const* eventName, OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(EventArgs&), char const* description = "", unsigned int flags = eEventFlag::EVENT_GLOBAL);
	
	void UnsubscribeEvent( std::string eventName, EventCallbackFunction functionPtr );
	template<typename OBJ_TYPE>
	void UnsubscribeObject(OBJ_TYPE* obj);
	template<typename OBJ_TYPE>
	void UnsubscribeMethodEvent(std::string const& eventName, OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(EventArgs&));

	void GetEventsFromFlag(unsigned int flags, std::vector<EventSubscription*>& events);

private:
	std::vector<EventSubscription*> m_subscriptions;
};


//////////////////////////////////////////////////////////////////////////
// class definitions
//////////////////////////////////////////////////////////////////////////
template<typename OBJ_TYPE>
EventSubscription::EventSubscription(char const* name, OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(EventArgs&), char const* desc, unsigned int eventFlags)
	:eventName(name)
	,description(desc)
	,flags(eventFlags)
{
    functionPtr.SubscribeMethod(obj, mcb);
}

//////////////////////////////////////////////////////////////////////////
template<typename OBJ_TYPE>
void EventSystem::RegisterMethodEvent(char const* eventName, OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(EventArgs&), char const* description /*= ""*/, unsigned int flags /*= eEventFlag::EVENT_GLOBAL*/)
{
	EventSubscription* newSub = new EventSubscription(eventName, obj, mcb, description, flags);
	m_subscriptions.push_back(newSub);
}

//////////////////////////////////////////////////////////////////////////
template<typename OBJ_TYPE>
void EventSystem::UnsubscribeObject(OBJ_TYPE* obj)
{
	for (size_t i = 0; i < m_subscriptions.size();) {
		EventSubscription* sub = m_subscriptions[i];
		sub->functionPtr.UnsubscribeObject(obj);
		if (sub->functionPtr.GetCallbackCount() < 1) {
			delete sub;
			m_subscriptions.erase(m_subscriptions.begin() + i);
		}
		else {
			i++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
template<typename OBJ_TYPE>
void EventSystem::UnsubscribeMethodEvent(std::string const& eventName, OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(EventArgs&))
{
	for (size_t i = 0; i < m_subscriptions.size(); i++) {
		EventSubscription* sub = m_subscriptions[i];
		if (sub->eventName == eventName) {
			sub->functionPtr.UnsubscribeMethod(obj, mcb);
            if (sub->functionPtr.GetCallbackCount() < 1) {
                delete sub;
                m_subscriptions.erase(m_subscriptions.begin()+i);
            }
            return;
		}
	}
}
