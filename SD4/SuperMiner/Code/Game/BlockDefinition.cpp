#include "BlockDefinition.hpp"

std::vector<BlockDefinition*> BlockDefinition::s_blockDefinitions = std::vector<BlockDefinition*>();

BlockDefinition::BlockDefinition(eBlockType ID, bool isFullyOpaque, AABB2 topUV, AABB2 sideUV, AABB2 botUVs)
{
	m_blockID = ID;
	m_isFullyOpaque = isFullyOpaque;
	m_topUVs = topUV;
	m_sideUVs = sideUV;
	m_bottomUVs = botUVs;

	s_blockDefinitions.push_back(this);
}

BlockDefinition * BlockDefinition::GetBlockDefinitionFromID(unsigned char blockID)
{
	for (BlockDefinition* definition : BlockDefinition::s_blockDefinitions)
	{
		if (definition->m_blockID == blockID){
			return definition;
		}
	}
	return nullptr;
}
