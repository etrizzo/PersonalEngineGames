#include "TileDefinition.hpp"


std::map<std::string, TileDefinition>	TileDefinition::s_definitions;

TileDefinition::TileDefinition(tinyxml2::XMLElement* tileDefElement)
{
	m_name					= ParseXmlAttribute(*tileDefElement, "name", "NO_NAME");		
	m_spriteCoords			= ParseXmlAttribute(*tileDefElement, "spriteCoords", IntVector2(0,0));
	m_texCoords				= g_blockSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoords);
	m_spriteTint			= ParseXmlAttribute(*tileDefElement, "spriteTint", RGBA::WHITE);
	m_allowsWalking			= ParseXmlAttribute(*tileDefElement, "allowsWalking", false);
	m_allowsSight			= ParseXmlAttribute(*tileDefElement, "allowsSight", false);
	m_allowsFlying			= ParseXmlAttribute(*tileDefElement, "allowsFlying", false);
	m_allowsSwimming		= ParseXmlAttribute(*tileDefElement, "allowsSwimming", false);
	m_isDestructible		= ParseXmlAttribute(*tileDefElement, "isDestructible", false);
	m_maxHealth				= ParseXmlAttribute(*tileDefElement, "maxHealth", 1.f);
	m_startingSpriteIndex	= GetIndexFromCoordinates(m_spriteCoords.x, m_spriteCoords.y, g_blockSpriteSheet->GetDimensions().x, g_blockSpriteSheet->GetDimensions().y);
}

AABB2 TileDefinition::GetTexCoordsAtHealth(int health)
{
	float percentage = (float)health/(float)m_maxHealth;
	int spriteIndex = Interpolate(m_startingSpriteIndex, m_startingSpriteIndex+3, 1-percentage);
	return g_blockSpriteSheet->GetTexCoordsForSpriteIndex(spriteIndex);
}

TileDefinition * TileDefinition::GetTileDefinition(std::string name)
{
	auto containsDef = s_definitions.find((std::string)name); 
	TileDefinition* tileDef = nullptr;
	if (containsDef != s_definitions.end()){
		tileDef = &containsDef->second;
	}
	return tileDef;
}

TileDefinition * ParseXmlAttribute(const tinyxml2::XMLElement & tileElement, const char* tileDefName, TileDefinition * defaultVal)
{
	std::string name = ParseXmlAttribute(tileElement, tileDefName, (std::string)"");
	TileDefinition* tileDef = TileDefinition::GetTileDefinition(name);
	if (tileDef == nullptr){
		tileDef = defaultVal;
	}
	return tileDef;
}
