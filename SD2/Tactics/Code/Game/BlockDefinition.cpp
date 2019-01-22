#include "BlockDefinition.hpp"


std::map<std::string, BlockDefinition>	BlockDefinition::s_definitions;

BlockDefinition::BlockDefinition(tinyxml2::XMLElement* tileDefElement)
{
	m_name					= ParseXmlAttribute(*tileDefElement, "name", "NO_NAME");		
	m_spriteCoordsTop		= ParseXmlAttribute(*tileDefElement, "spriteCoordsTop", IntVector2(0,0));
	m_spriteCoordsSide		= ParseXmlAttribute(*tileDefElement, "spriteCoordsSide", IntVector2(0,0));
	m_spriteCoordsBottom	= ParseXmlAttribute(*tileDefElement, "spriteCoordsBottom", IntVector2(0,0));
	m_uvTop					= g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoordsTop);
	m_uvSide				= g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoordsSide);
	m_uvBottom				= g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoordsBottom);
	m_spriteTint			= ParseXmlAttribute(*tileDefElement, "spriteTint", RGBA::WHITE);
	m_allowsWalking			= ParseXmlAttribute(*tileDefElement, "allowsWalking", false);
	m_allowsSight			= ParseXmlAttribute(*tileDefElement, "allowsSight", false);
	m_allowsFlying			= ParseXmlAttribute(*tileDefElement, "allowsFlying", false);
	m_allowsSwimming		= ParseXmlAttribute(*tileDefElement, "allowsSwimming", false);
	m_isDestructible		= ParseXmlAttribute(*tileDefElement, "isDestructible", false);
	m_maxHealth				= ParseXmlAttribute(*tileDefElement, "maxHealth", 1.f);
	//m_startingSpriteIndex	= GetIndexFromCoordinates(m_spriteCoords.x, m_spriteCoords.y, g_tileSpriteSheet->GetDimensions().x, g_tileSpriteSheet->GetDimensions().y);
}

void BlockDefinition::MakeGrassBlockDefinition()
{
	m_name					= "Grass";
	m_spriteCoordsTop		= IntVector2(21,0);
	m_spriteCoordsSide		= IntVector2(3,3);
	m_spriteCoordsBottom	= IntVector2(4,3);
	m_uvTop					= g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoordsTop);
	m_uvSide				= g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoordsSide);
	m_uvBottom				= g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoordsBottom);
	m_spriteTint			= RGBA::WHITE;
	m_allowsWalking			= true;
	m_allowsSight			= false;
	m_allowsFlying			= true;
	m_allowsSwimming		= false;
	m_isDestructible		= false;
	m_maxHealth				= 1;
}

BlockDefinition * BlockDefinition::GetTileDefinition(std::string name)
{
	auto containsDef = s_definitions.find((std::string)name); 
	BlockDefinition* tileDef = nullptr;
	if (containsDef != s_definitions.end()){
		tileDef = &containsDef->second;
	}
	return tileDef;
}

BlockDefinition * ParseXmlAttribute(const tinyxml2::XMLElement & tileElement, const char* tileDefName, BlockDefinition * defaultVal)
{
	std::string name = ParseXmlAttribute(tileElement, tileDefName, (std::string)"");
	BlockDefinition* tileDef = BlockDefinition::GetTileDefinition(name);
	if (tileDef == nullptr){
		tileDef = defaultVal;
	}
	return tileDef;
}
