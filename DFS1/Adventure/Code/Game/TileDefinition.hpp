#pragma once
#include "Game/GameCommon.hpp"
#include "Game/TileEdgeDefinition.hpp"

enum eTerrainTypes{
	TERRAIN_GROUND,
	TERRAIN_GRASS,
	TERRAIN_WATER,
	NUM_TERRAIN_TYPES
};

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
	bool m_isTerrain;
	int m_terrainLevel;
	int m_maxHealth;
	int m_startingSpriteIndex;
	TileEdgeDefinition* m_edgeDefinition;

	AABB2 GetTexCoords(int index = 0);
	AABB2 GetTexCoordsAtHealth(int health);

	static std::map <std::string, TileDefinition*>	s_definitions;
	static TileDefinition* GetTileDefinition(std::string name);
	static TileDefinition* GetTileDefinitionByChroma(RGBA chroma);
private:
	void ParseSpriteCoords(const tinyxml2::XMLElement* tileElement);
};



TileDefinition* ParseXmlAttribute(const tinyxml2::XMLElement& tileElement, const char* tileDefName, TileDefinition* defaultVal);