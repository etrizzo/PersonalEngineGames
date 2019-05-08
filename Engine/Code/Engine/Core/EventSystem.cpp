#include "EventSystem.hpp"

EventSystem* EventSystem::s_theEventSystem = nullptr;


EventSystem * EventSystem::GetInstance()
{
	if (s_theEventSystem == nullptr) {
		s_theEventSystem = new EventSystem();
	}
	return s_theEventSystem;
}

void EventSystem::FireEvent(std::string name, NamedProperties& args)
{
	
	auto hasSubscription = m_subscriptions.find(name);
	if (hasSubscription != m_subscriptions.end()) {
		
		for (EventSubscription* sub : hasSubscription->second)
		{
			sub->Execute(args);
		}
	}
}

void EventSystem::SubscribeEventCallbackFunction(std::string name, EventFunctionCallbackPtrType function)
{
	EventSubscription* newSub = (EventSubscription*) new EventFunctionSubscription(function);
	auto hasSubscription = m_subscriptions.find(name);
	if (hasSubscription != m_subscriptions.end()) {

		hasSubscription->second.push_back(newSub); 
	}
	else {
		std::vector<EventSubscription*> list = std::vector<EventSubscription*>();
		list.push_back(newSub);
		m_subscriptions[name] = list;
	}
}

void EventSystem::UnsubscribeEventCallbackFunction(std::string name, EventFunctionCallbackPtrType function)
{
	auto hasSubscription = m_subscriptions.find(name);
	if (hasSubscription != m_subscriptions.end()) {
		std::vector<EventSubscription*> subList = hasSubscription->second;
		for (int i = subList.size() - 1; i >= 0; i--)
		{
			EventFunctionSubscription* functionSub = dynamic_cast<EventFunctionSubscription*>(subList[i]);
			if (functionSub != nullptr)
			{
				//we have a function subscription, compare to the function we passed in
				if (functionSub->m_functionPointer == function)
				{
					//it's subscribed - gotta delete it
					//RemoveAtFast
					if (subList.size() > 1) {
						subList[i] = subList[subList.size() - 1];
					}
					subList.pop_back();
				}
			}
		}
	}
}
