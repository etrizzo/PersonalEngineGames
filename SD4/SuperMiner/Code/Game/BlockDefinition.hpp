#pragma once
#include "Game/GameCommon.hpp"

class BlockDefinition
{
public:
	BlockDefinition(eBlockType ID, bool isFullyOpaque, AABB2 topUV, AABB2 sideUV, AABB2 botUVs);

	bool IsFullyOpaque() const		{ return m_isFullyOpaque; };
	
	eBlockType m_blockID = BLOCK_AIR;
	bool m_isFullyOpaque = true;

	AABB2 m_topUVs;
	AABB2 m_sideUVs;
	AABB2 m_bottomUVs;

	static BlockDefinition* GetBlockDefinitionFromID(unsigned char blockID);

	static std::vector<BlockDefinition*> s_blockDefinitions;
};