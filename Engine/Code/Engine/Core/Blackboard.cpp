#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/EngineCommon.hpp"

Blackboard::Blackboard()
{
	m_keyValuePairs = std::map< std::string, std::string >();
}

Blackboard::~Blackboard()
{

}

void Blackboard::PopulateFromXmlElementAttributes(const tinyxml2::XMLElement & element)
{
	
	for( const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != 0; attribute=attribute->Next()) {
		std::string name = attribute->Name();
		std::string value = attribute->Value();
		m_keyValuePairs.insert(std::pair<std::string, std::string> (name, value));
	}
}

void Blackboard::SetValue(const std::string & keyName, const std::string & newValue)
{
	m_keyValuePairs.insert(std::pair<std::string, std::string> (keyName, newValue));
}

bool Blackboard::GetValue(const std::string & keyName, bool defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	bool value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string boolAsString = containsDef->second;
		value = ParseStringBool(boolAsString);
	}
	return value;
}

int Blackboard::GetValue(const std::string & keyName, int defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	int value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string intAsString = containsDef->second;
		value = ParseStringInt(intAsString);
	}
	return value;
}

float Blackboard::GetValue(const std::string & keyName, float defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	float value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string floatAsString = containsDef->second;
		value = ParseStringFloat(floatAsString);
	}
	return value;
}

std::string Blackboard::GetValue(const std::string & keyName, std::string defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	std::string value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		value = containsDef->second;
	}
	return value;
}

std::string Blackboard::GetValue(const std::string & keyName, const char * defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	std::string value = (std::string) defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		value = containsDef->second;
	}
	return value;
}

RGBA Blackboard::GetValue(const std::string & keyName, const RGBA & defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	RGBA value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string RGBAAsString = containsDef->second;
		value.SetFromText(RGBAAsString.c_str());
	}
	return value;
}

Vector2 Blackboard::GetValue(const std::string & keyName, const Vector2 & defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	Vector2 value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string vec2AsString = containsDef->second;
		value.SetFromText(vec2AsString.c_str());
	}
	return value;
}

IntVector2 Blackboard::GetValue(const std::string & keyName, const IntVector2 & defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	IntVector2 value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string vec2AsString = containsDef->second;
		value.SetFromText(vec2AsString.c_str());
	}
	return value;
}

FloatRange Blackboard::GetValue(const std::string & keyName, const FloatRange & defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	FloatRange value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string rangeAsString = containsDef->second;
		value.SetFromText(rangeAsString.c_str());
	}
	return value;
}

IntRange Blackboard::GetValue(const std::string & keyName, const IntRange & defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	IntRange value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string rangeAsString = containsDef->second;
		value.SetFromText(rangeAsString.c_str());
	}
	return value;
}

Vector3 Blackboard::GetValue(const std::string & keyName, const Vector3 & defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	Vector3 value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string vec2AsString = containsDef->second;
		value.SetFromText(vec2AsString.c_str());
	}
	return value;
}

IntVector3 Blackboard::GetValue(const std::string & keyName, const IntVector3 & defaultValue) const
{
	auto containsDef = m_keyValuePairs.find(keyName); 
	IntVector3 value = defaultValue;
	if (containsDef != m_keyValuePairs.end()){
		std::string vec2AsString = containsDef->second;
		value.SetFromText(vec2AsString.c_str());
	}
	return value;
}
