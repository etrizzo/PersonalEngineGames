#include "Game/Block.hpp"
#include "Game/BlockDefinition.hpp"

Block::Block()
{
	m_blockID = 0U;
	m_bits = 0U;
	m_lighting = 0U;
}

Block::Block(uchar blockType)
{
	m_blockID = blockType;
	m_bits = 0U;
	m_lighting = 0U;
}

void Block::SetType(uchar newType)
{
	m_blockID = newType;
	BlockDefinition* newDefinition = BlockDefinition::GetBlockDefinitionFromID(newType);

	//m_lighting = (uchar) newDefinition->m_internalLightLevel;

	if (newDefinition->IsFullyOpaque())
	{
		SetBits(m_bits, BLOCK_BIT_IS_FULL_OPAQUE);
	}
	else {
		ClearBits(m_bits, BLOCK_BIT_IS_FULL_OPAQUE);
	}
}

BlockDefinition * Block::GetType() const
{
	return BlockDefinition::GetBlockDefinitionFromID(m_blockID);
}


uchar Block::GetDefinitionInternalLightLevel() const
{
	return (uchar) GetType()->m_internalLightLevel;
}
