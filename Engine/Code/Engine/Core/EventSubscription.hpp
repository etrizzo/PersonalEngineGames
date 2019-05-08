#pragma once
#include "Engine/Core/NamedProperties.hpp"

class EventSubscription
{
public:
	virtual bool Execute(NamedProperties& args) = 0;
};


//note: the bool is (later) a signal of whether the event should be consumed by me
typedef bool(*EventFunctionCallbackPtrType) (NamedProperties* args);

class EventFunctionSubscription : public EventSubscription
{
public:
	EventFunctionSubscription(EventFunctionCallbackPtrType function);

	virtual bool Execute(NamedProperties& args) override
	{
		return m_functionPointer(&args);
	}

	//this is not a pointer, because the typedef is already a pointer.
	EventFunctionCallbackPtrType m_functionPointer = nullptr;

};