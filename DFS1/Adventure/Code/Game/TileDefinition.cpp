#include "TileDefinition.hpp"
#include "Engine/Core/RGBA.hpp"


std::map<std::string, TileDefinition*>	TileDefinition::s_definitions;

TileDefinition::TileDefinition(tinyxml2::XMLElement* tileDefElement)
{
	m_name					= ParseXmlAttribute(*tileDefElement, "name", "NO_NAME");	

	ParseSpriteCoords(tileDefElement);
	m_spriteTint			= ParseXmlAttribute(*tileDefElement, "spriteTint", RGBA::WHITE);
	m_tileChromaKey			= ParseXmlAttribute(*tileDefElement, "chromaKey", RGBA::WHITE);
	m_allowsWalking			= ParseXmlAttribute(*tileDefElement, "allowsWalking", false);
	m_allowsSight			= ParseXmlAttribute(*tileDefElement, "allowsSight", false);
	m_allowsFlying			= ParseXmlAttribute(*tileDefElement, "allowsFlying", false);
	m_allowsSwimming		= ParseXmlAttribute(*tileDefElement, "allowsSwimming", false);
	m_isDestructible		= ParseXmlAttribute(*tileDefElement, "isDestructible", false);
	m_maxHealth				= ParseXmlAttribute(*tileDefElement, "maxHealth", 1);
	m_startingSpriteIndex	= GetIndexFromCoordinates(m_spriteCoords[0].x, m_spriteCoords[0].y, g_tileSpriteSheet->GetDimensions().x, g_tileSpriteSheet->GetDimensions().y);
}

AABB2 TileDefinition::GetTexCoords(int index)
{
	return g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoords[index]);
}

AABB2 TileDefinition::GetTexCoordsAtHealth(int health)
{
	float percentage = (float)health/(float)m_maxHealth;
	int spriteIndex = Interpolate(m_startingSpriteIndex, m_startingSpriteIndex+3, 1-percentage);
	return g_tileSpriteSheet->GetTexCoordsForSpriteIndex(spriteIndex);
}

TileDefinition * TileDefinition::GetTileDefinition(std::string name)
{
	auto containsDef = s_definitions.find((std::string)name); 
	TileDefinition* tileDef = nullptr;
	if (containsDef != s_definitions.end()){
		tileDef = containsDef->second;
	}
	return tileDef;
}

TileDefinition * TileDefinition::GetTileDefinitionByChroma(RGBA chroma)
{
	for( const auto tilePair : s_definitions )
	{
		TileDefinition* tileDef = tilePair.second;
		if (chroma.CompareWithoutAlpha(tileDef->m_tileChromaKey)){
			return tileDef;
		}
	}
	return nullptr;
}

void TileDefinition::ParseSpriteCoords(const tinyxml2::XMLElement * tileElement)
{
	std::string spriteCoordString = ParseXmlAttribute(*tileElement, "spriteCoords", "");
	Strings splitCoords = Strings();
	Split(spriteCoordString, '|', splitCoords);
	for(std::string coord : splitCoords){
		IntVector2 spriteCoord = IntVector2();
		spriteCoord.SetFromText(coord.c_str());
		m_spriteCoords.push_back(spriteCoord);
	}
}

TileDefinition * ParseXmlAttribute(const tinyxml2::XMLElement & tileElement, const char* tileDefName, TileDefinition * defaultVal)
{
	std::string name = ParseXmlAttribute(tileElement, tileDefName, (std::string)"");
	TileDefinition* tileDef = TileDefinition::GetTileDefinition(name);
	if (tileDef == nullptr){
		return defaultVal;
	}

	return tileDef;
}
