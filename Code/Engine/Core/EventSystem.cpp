#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedProperties.hpp"

static EventSubscription* g_eventSubscriptionList[MAX_REGISTERED_EVENTS];
static unsigned int g_eventSubscriptionCount = 0;

//////////////////////////////////////////////////////////////////////////
EventSystem::EventSystem()
{
	for (unsigned int idx = 0; idx < g_eventSubscriptionCount; idx++) {
		EventSubscription* subscription = g_eventSubscriptionList[idx];
		m_subscriptions.push_back(subscription);
	}
}

//////////////////////////////////////////////////////////////////////////
bool EventSystem::FireEvent( std::string const& eventRawString, unsigned int flags )
{
	//process raw string
    NamedProperties eventParameters;
	std::string eventName;
	if (!ParseEventRawString(Trim(eventRawString), eventName, eventParameters)) {
		return false;
	}

	return FireEvent(eventName,eventParameters,flags);
}

//////////////////////////////////////////////////////////////////////////
bool EventSystem::FireEvent(std::string const& eventName, NamedProperties& parameters, unsigned int flags/*=eEventFlag::EVENT_GLOBAL*/)
{
	//call event
    for (EventSubscription* subscription : m_subscriptions) {
        if (subscription->eventName == eventName && (subscription->flags & flags)) {
            if (subscription->functionPtr.Invoke(parameters)) {
                return true;
            }
            else {
                g_theConsole->PrintString(Rgba8::MAGENTA, Stringf("Executing %s failed", eventName.c_str()));
            }
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
void EventSystem::RegisterEvent( std::string const& eventName, EventCallbackFunction delegates, std::string const& description, unsigned int flags)
{
	EventSubscription* newSubscription = new EventSubscription(eventName.c_str(),delegates,description.c_str(),flags);
	m_subscriptions.push_back( newSubscription );
}

//////////////////////////////////////////////////////////////////////////
void EventSystem::UnsubscribeEvent( std::string eventName, EventCallbackFunction delegates )
{
	for( std::vector<EventSubscription*>::iterator subscriptionIter = m_subscriptions.begin();subscriptionIter!=m_subscriptions.end();subscriptionIter++ )
	{
		EventSubscription* subscription = *subscriptionIter;
		if( subscription->eventName == eventName)
		{
			subscription->functionPtr.Unsubscribe(delegates);
			if (subscription->functionPtr.GetCallbackCount() < 1) {
				delete subscription;
				m_subscriptions.erase(subscriptionIter);
			}
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void EventSystem::GetEventsFromFlag(unsigned int flags, std::vector<EventSubscription*>& events)
{
	for (size_t idx = 0; idx < m_subscriptions.size(); idx++) {
		EventSubscription* sub = m_subscriptions[idx];
		if (sub->flags & flags) {
			events.push_back(sub);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
EventSubscription::EventSubscription(char const* name, EventCallbackFunction fPtr, char const* desc, unsigned int eventFlags)
	:eventName(name)
	,description(desc)
	,flags(eventFlags)
{
	functionPtr.Subscribe(fPtr);
	g_eventSubscriptionList[g_eventSubscriptionCount] = this;
	++g_eventSubscriptionCount;
}
