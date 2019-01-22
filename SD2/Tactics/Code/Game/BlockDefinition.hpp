#pragma once
#include "Game/GameCommon.hpp"





class BlockDefinition{
public:
	BlockDefinition() {};
	BlockDefinition(tinyxml2::XMLElement* tileDefElement);



	std::string m_name;
	Texture* m_texture;
	IntVector2 m_spriteCoordsTop;
	IntVector2 m_spriteCoordsSide;
	IntVector2 m_spriteCoordsBottom;
	AABB2 m_uvTop;
	AABB2 m_uvSide;
	AABB2 m_uvBottom;
	RGBA m_spriteTint;
	bool m_allowsWalking;
	bool m_allowsSight;
	bool m_allowsFlying;
	bool m_allowsSwimming;
	bool m_isDestructible;
	float m_maxHealth;
	//int m_startingSpriteIndex;

	void MakeGrassBlockDefinition();

	static std::map <std::string, BlockDefinition>	s_definitions;
	static BlockDefinition* GetTileDefinition(std::string name);

};



BlockDefinition* ParseXmlAttribute(const tinyxml2::XMLElement& tileElement, const char* tileDefName, BlockDefinition* defaultVal);