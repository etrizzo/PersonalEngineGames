#pragma once
#include "Game/GameCommon.hpp"





class TileDefinition{
public:
	TileDefinition() {};
	TileDefinition(tinyxml2::XMLElement* tileDefElement);



	std::string m_name;
	IntVector2 m_spriteCoords;
	AABB2 m_texCoords;
	RGBA m_spriteTint;
	bool m_allowsWalking;
	bool m_allowsSight;
	bool m_allowsFlying;
	bool m_allowsSwimming;
	bool m_isDestructible;
	float m_maxHealth;
	int m_startingSpriteIndex;

	AABB2 GetTexCoordsAtHealth(int health);

	static std::map <std::string, TileDefinition>	s_definitions;
	static TileDefinition* GetTileDefinition(std::string name);

};



TileDefinition* ParseXmlAttribute(const tinyxml2::XMLElement& tileElement, const char* tileDefName, TileDefinition* defaultVal);