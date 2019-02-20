#include "Game/Block.hpp"
#include "Game/BlockDefinition.hpp"

Block::Block()
{
	m_blockID = 0U;
	m_bits = 0U;
}

Block::Block(eBlockType blockType)
{
	m_blockID = (unsigned char) blockType;
	m_bits = 0U;
}

void Block::SetType(eBlockType newType)
{
	m_blockID = newType;
}

BlockDefinition * Block::GetType() const
{
	return BlockDefinition::GetBlockDefinitionFromID(m_blockID);
}

bool Block::IsFullyOpaque() const
{
	return GetType()->IsFullyOpaque();
}
