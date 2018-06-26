#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "ThirdParty/TinyXML/tinyxml2.h"
#include <string>



int ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue );
char ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue );
bool ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue );
float ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue );
RGBA ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const RGBA& defaultValue );
Vector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vector2& defaultValue );
IntRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const FloatRange& defaultValue );
IntVector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntVector2& defaultValue );
AABB2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const AABB2& defaultValue );
std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue );
std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const char* defaultValue=nullptr );
Texture* ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, Texture* defaultValue=nullptr );


bool ParseStringBool(std::string stringToParse);
int ParseStringInt(std::string stringToParse);
float ParseStringFloat(std::string stringToParse);
char ParseStringChar(std::string stringToParse);