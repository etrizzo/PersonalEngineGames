#include "Engine/Core/NamedPropertyBase.hpp"

class NamedProperties
{
public:
	template<typename T>
	void Set(std::string key, T value)		//this means that you have 0 to ???? different Set() functions on NamedProperties
	{
		auto hasProperty = m_properties.find(key);
		if (hasProperty != m_properties.end()) {
			NamedPropertyBase* property = hasProperty->second;
			//we contain a property with that key, now we have to verify that it's the right type.
			//Solving with Dyanmic Cast
			//basically, we use dynamic_cast to assume that it's the type we're requesting
			//property right now to us is TypedProperty<???>, we need to see if it's TypedProperty<T>
			TypedProperty<T>* asRequestedType = dynamic_cast<TypedProperty<T>*> (property);
			if (asRequestedType != nullptr)
			{
				//dynamic cast actually worked. We can confidently say the TypedProperty is a TypedProperty<T>
				asRequestedType->m_value = value;
				//note: do we need to re-assign to the map? do we need to delete from the map?
				//m_properties[key] = (NamedPropertyBase*)asRequestedType;
			} else
			{
				//the key already existed, but was a different type...
				//delete the existing value
				delete m_properties[key];
				TypedProperty<T>* newValue = new TypedProperty<T>();
				newValue->m_value = value;
				m_properties[key] = (NamedPropertyBase*)newValue;
			}
		}
		else
		{
			TypedProperty<T>* newValue = new TypedProperty<T>();
			newValue->m_value = value;
			m_properties[key] = (NamedPropertyBase*) newValue;
		}
	}

	void Set(const char* keyName, const char* value)
	{
		Set<std::string>(keyName, std::string(value));
	}


	template<typename T>
	T Get(std::string key, T defaultValue) const
	{
		auto hasProperty = m_properties.find(key);
		if (hasProperty != m_properties.end()) {
			NamedPropertyBase* property = hasProperty->second;
			//we contain a property with that key, now we have to verify that it's the right type.
			//Solving with Dyanmic Cast
			//basically, we use dynamic_cast to assume that it's the type we're requesting
			//property right now to us is TypedProperty<???>, we need to see if it's TypedProperty<T>
			TypedProperty<T>* asRequestedType = dynamic_cast<TypedProperty<T>*> (property);
			if (asRequestedType != nullptr)
			{
				//dynamic cast actually worked. We can confidently say the TypedProperty is a TypedProperty<T>
				return asRequestedType->m_value;
			}
		}
		//return the default value
		return defaultValue;
	}


	std::string Get(const char* keyName, const char* defaultValue)
	{
		return Get<std::string>(keyName, std::string(defaultValue));
	}


	std::map<std::string, NamedPropertyBase* > m_properties;
};