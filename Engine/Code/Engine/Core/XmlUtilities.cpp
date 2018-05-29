#include "XmlUtilities.hpp"

int ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, int defaultValue)
{
	int tempVal = defaultValue;
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	if (parsedString != " "){
		tempVal =ParseStringInt(parsedString);
	}
	return tempVal;
}

char ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, char defaultValue)
{
	char tempVal = defaultValue;
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	if (parsedString != " "){
		tempVal = ParseStringChar(parsedString);
	}
	return tempVal;
}

bool ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, bool defaultValue)
{
	bool tempBool = defaultValue;
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	if (parsedString != " "){
		tempBool = ParseStringBool(parsedString);
	}
	return tempBool;
}

float ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, float defaultValue)
{
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	float tempFloat = defaultValue;
	if (parsedString != " "){
		tempFloat = ParseStringFloat(parsedString);
	}
	return tempFloat;
}

RGBA ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const RGBA & defaultValue)
{
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	RGBA tempColor = RGBA(defaultValue);
	if (parsedString != " "){
		tempColor.SetFromText(parsedString.c_str());
	}
	return tempColor;
}

Vector2 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const Vector2 & defaultValue)
{
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	Vector2 tempVector = Vector2(defaultValue);
	if (parsedString != " "){
		tempVector.SetFromText(parsedString.c_str());
	}
	return tempVector;
}

IntRange ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const IntRange & defaultValue)
{
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	IntRange tempRange = IntRange(defaultValue);
	if (parsedString != " "){
		tempRange.SetFromText(parsedString.c_str());
	}
	return tempRange;
}

FloatRange ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const FloatRange & defaultValue)
{
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	FloatRange tempRange = FloatRange(defaultValue);
	if (parsedString != " "){
		tempRange.SetFromText(parsedString.c_str());
	}
	return tempRange;
}

IntVector2 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const IntVector2 & defaultValue)
{
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	IntVector2 tempVector = IntVector2(defaultValue);
	if (parsedString != " "){
		tempVector.SetFromText(parsedString.c_str());
	}
	return tempVector;
}

AABB2 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const AABB2 & defaultValue)
{
	std::string parsedString = ParseXmlAttribute(element, attributeName, std::string(" "));
	AABB2 tempBox = AABB2(defaultValue);
	if (parsedString != " "){
		tempBox.SetFromText(parsedString.c_str());
	}
	return tempBox;
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const std::string & defaultValue)
{
	std::string temp = defaultValue;
	const char* attributeVal = element.Attribute(attributeName);

	if (attributeVal != NULL){
		temp = (std::string) attributeVal;
	}

	return temp;
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const char * defaultValue)
{
	return ParseXmlAttribute(element,attributeName, std::string(defaultValue));
}

bool ParseStringBool(std::string stringToParse)
{
	return (stringToParse == "true");;
}

int ParseStringInt(std::string stringToParse)
{
	//return (int) strtol(stringToParse.c_str(), (char **)NULL, 10);

	return atoi(stringToParse.c_str());
}

float ParseStringFloat(std::string stringToParse)
{
	return (float) atof(stringToParse.c_str());
}

char ParseStringChar(std::string stringToParse)
{
	return stringToParse.c_str()[0];
}
