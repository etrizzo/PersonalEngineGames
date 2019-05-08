#pragma once
#include "Engine/Core/EventSubscription.hpp"
#include <vector>


class EventSystem
{
public:
	static EventSystem* GetInstance();
	std::map<std::string, std::vector<EventSubscription*> > m_subscriptions;

	void FireEvent(std::string name, NamedProperties& args);
	void SubscribeEventCallbackFunction(std::string name, EventFunctionCallbackPtrType function);
	void UnsubscribeEventCallbackFunction(std::string name, EventFunctionCallbackPtrType function);

private:
	static EventSystem* s_theEventSystem;
};


