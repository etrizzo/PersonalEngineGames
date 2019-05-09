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



template <typename T>
class EventObjMethodSub : public EventSubscription
{
public:
	typedef bool (T::*EventObjectMethodCallbackPtrType) (NamedProperties* eventArgs);

	EventObjMethodSub(T* object, EventObjectMethodCallbackPtrType method)
		: m_object(object)
		, m_method(method)
	{};

	virtual bool Execute( NamedProperties& args ) override
	{
		// we need our specific instance to call our function...
		(m_object->*m_method)(&args);		//just like don't even worry about this we'll talk about it later idk
										//only ever used if we have a variable method we want to call on a specific instance.
		return false;
	};

public:
	T* m_object;
	//Game::Method(...) or Thing::Method(...)
	EventObjectMethodCallbackPtrType m_method;		//how do we even do this tho
};