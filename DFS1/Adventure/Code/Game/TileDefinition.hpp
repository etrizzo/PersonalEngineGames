#pragma once
#include "Game/GameCommon.hpp"





class TileDefinition{
public:
	TileDefinition() {};
	TileDefinition(tinyxml2::XMLElement* tileDefElement);



	std::string m_name;
	std::vector<IntVector2> m_spriteCoords;
	RGBA m_spriteTint;
	RGBA m_tileChromaKey;
	bool m_allowsWalking;
	bool m_allowsSight;
	bool m_allowsFlying;
	bool m_allowsSwimming;
	bool m_isDestructible;
	int m_maxHealth;
	int m_startingSpriteIndex;

	AABB2 GetTexCoords(int index = 0);
	AABB2 GetTexCoordsAtHealth(int health);

	static std::map <std::string, TileDefinition*>	s_definitions;
	static TileDefinition* GetTileDefinition(std::string name);
	static TileDefinition* GetTileDefinitionByChroma(RGBA chroma);
private:
	void ParseSpriteCoords(const tinyxml2::XMLElement* tileElement);
};



TileDefinition* ParseXmlAttribute(const tinyxml2::XMLElement& tileElement, const char* tileDefName, TileDefinition* defaultVal);