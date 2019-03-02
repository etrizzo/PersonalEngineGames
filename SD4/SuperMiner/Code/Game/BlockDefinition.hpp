#pragma once
#include "Game/GameCommon.hpp"

class BlockDefinition
{
public:
	BlockDefinition(tinyxml2::XMLElement* blockElement);
	BlockDefinition(uchar ID, bool isFullyOpaque, AABB2 topUV, AABB2 sideUV, AABB2 botUVs);

	bool IsFullyOpaque() const		{ return m_isFullyOpaque; };
	
	std::string m_name = "Air";
	uchar m_blockID = BLOCK_AIR;
	bool m_isFullyOpaque = true;

	int m_internalLightLevel = 0U;

	AABB2 m_topUVs		= AABB2::ZERO_TO_ONE;
	AABB2 m_sideUVs		= AABB2::ZERO_TO_ONE;
	AABB2 m_bottomUVs	= AABB2::ZERO_TO_ONE;

	static BlockDefinition* GetBlockDefinitionFromID(unsigned char blockID);
	static uchar GetBlockIDFromName(const std::string& name);
	static std::vector<BlockDefinition*> s_blockDefinitions;
};