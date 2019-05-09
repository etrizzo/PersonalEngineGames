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

	template <typename T, typename METHOD>
	void SubscribeEventCallbackObjectMethod(std::string name, T* object, METHOD function)
	{
		typedef bool (T::*EventObjectMethodCallbackPtrType) (NamedProperties* eventArgs);

		EventObjectMethodCallbackPtrType methodActually = static_cast<EventObjectMethodCallbackPtrType>(function);
		if (methodActually != nullptr)
		{
			EventSubscription* newSub = (EventSubscription*) new EventObjMethodSub<T>(object, methodActually);
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
	};

	template <typename T, typename METHOD>
	void UnsubscribeEventCallbackObjectMethod(std::string name, T* object, METHOD function)
	{
		typedef bool (T::*EventObjectMethodCallbackPtrType) (NamedProperties* eventArgs);

		auto hasSubscription = m_subscriptions.find(name);
		if (hasSubscription != m_subscriptions.end()) {
			//std::vector<EventSubscription*> subList = hasSubscription->second;
			for (int i = m_subscriptions[name].size() - 1; i >= 0; i--)
			{
				EventObjectMethodCallbackPtrType methodActually = static_cast<EventObjectMethodCallbackPtrType>(function);
				EventObjMethodSub<T>* functionSub = dynamic_cast<EventObjMethodSub<T>*>(m_subscriptions[name][i]);
				if (functionSub != nullptr)
				{
					//we have a function subscription, compare the object and method to what we have subscribed
					if (object == functionSub->m_object && functionSub->m_method == methodActually)
					{
						//it's subscribed - gotta delete it
						//RemoveAtFast
						if (m_subscriptions[name].size() > 1) {
							m_subscriptions[name][i] = m_subscriptions[name][m_subscriptions[name].size() - 1];
						}
						m_subscriptions[name].pop_back();
					}
				}
			}
		}

	};

private:
	static EventSystem* s_theEventSystem;
};


