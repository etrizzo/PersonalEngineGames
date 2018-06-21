#include "DecorationDefinition.hpp"
#include "Game/Stats.hpp"

std::map<std::string, DecorationDefinition*>	DecorationDefinition::s_definitions;

DecorationDefinition::DecorationDefinition(tinyxml2::XMLElement * decoElement)
	:EntityDefinition(decoElement)
{
	tinyxml2::XMLElement* collisionElement = decoElement->FirstChildElement("Collision");
	m_allowsFlying		= ParseXmlAttribute(*collisionElement, "allowsFlying", m_allowsFlying);
	m_allowsSwimming	= ParseXmlAttribute(*collisionElement, "allowsSwimming", m_allowsSwimming);
	m_allowsWalking		= ParseXmlAttribute(*collisionElement, "allowsWalking", m_allowsWalking);
}

DecorationDefinition::~DecorationDefinition()
{

}

DecorationDefinition * DecorationDefinition::GetDecorationDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	DecorationDefinition* itemDef = nullptr;
	if (containsDef != s_definitions.end()){
		itemDef = containsDef->second;
	} else {
		ERROR_AND_DIE("No decoration named: " + definitionName);
	}
	return itemDef;
}


