#include "Game/BlockLocator.hpp"
#include "Game/Chunk.hpp"

BlockLocator::BlockLocator(int blockIndex, Chunk* chunk)
{
	m_blockIndex = blockIndex;
	m_chunk = chunk;
}

bool BlockLocator::operator==(const BlockLocator& other)
{
	return (m_chunk == other.m_chunk) && (m_blockIndex == other.m_blockIndex);
}

bool BlockLocator::operator!=(const BlockLocator& other)
{
	return (m_chunk != other.m_chunk) || (m_blockIndex != other.m_blockIndex);
}

Block & BlockLocator::GetBlock() const
{
	if (m_chunk == nullptr)
	{
		return g_invalidBlock;
	} else {
		return m_chunk->GetBlock(m_blockIndex);
	}
}

BlockDefinition * BlockLocator::GetBlockType() const
{
	return GetBlock().GetType();
}


AABB3 BlockLocator::GetBlockBounds() const
{
	return AABB3(GetBlockCenterWorldPosition(), .5f, .5f, .5f);
}

Vector3 BlockLocator::GetBlockCenterWorldPosition() const
{
	if (m_chunk != nullptr)
	{
		IntVector3 localBlockCoords = m_chunk->GetBlockCoordinatesForBlockIndex(m_blockIndex);
		IntVector3 worldBlockCoords = localBlockCoords + IntVector3(m_chunk->GetChunkCoords().x * CHUNK_SIZE_X, m_chunk->GetChunkCoords().y * CHUNK_SIZE_Y, 0);
		Vector3 blockCenter = worldBlockCoords.GetVector3() + Vector3::HALF;
		return blockCenter;
	}
	return Vector3::ZERO;
}




BlockLocator BlockLocator::GetEast() const
{
	if (IsBlockIndexOnEastEdge(m_blockIndex)){
		Chunk* neighborChunk = m_chunk->m_eastNeighbor;
		//x is now the westernmost block, which is 0
		int newIndex = m_blockIndex & (~CHUNK_MASK_X);		//want to just like shwoop out the x mask
		return BlockLocator(newIndex, neighborChunk);
	} else {
		return BlockLocator(m_blockIndex + 1, m_chunk);
	}
}

BlockLocator BlockLocator::GetWest() const
{
	if (IsBlockIndexOnWestEdge(m_blockIndex)){
		Chunk* neighborChunk = m_chunk->m_westNeighbor;
		//IntVector3 newBlockCoords = IntVector3(0, Chunk::GetBlockYCoordinate(m_blockIndex), Chunk::GetBlockZCoordinate(m_blockIndex));
		//x is now the easternmost block, which is 15
		int newIndex = m_blockIndex | CHUNK_MASK_X;		//want to just like shwoop in the x mask
		return BlockLocator(newIndex, neighborChunk);
	} else {
		return BlockLocator(m_blockIndex - 1, m_chunk);
	}
}

BlockLocator BlockLocator::GetSouth() const
{
	if (IsBlockIndexOnSouthEdge(m_blockIndex)){
		Chunk* neighborChunk = m_chunk->m_southNeighbor;
		//y is now the northern block, which is 15
		int newIndex = m_blockIndex | CHUNK_MASK_Y;		//want to just like shwoop out the x mask
		return BlockLocator(newIndex, neighborChunk);
	} else {
		return BlockLocator(m_blockIndex - CHUNK_SIZE_Y, m_chunk);
	}
}

BlockLocator BlockLocator::GetNorth() const
{
	if (IsBlockIndexOnNorthEdge(m_blockIndex)){
		Chunk* neighborChunk = m_chunk->m_northNeighbor;
		//y is now the southern block, which is 0
		int newIndex = m_blockIndex & (~CHUNK_MASK_Y);		//want to just like shwoop out the x mask
		return BlockLocator(newIndex, neighborChunk);
	} else {
		return BlockLocator(m_blockIndex + CHUNK_SIZE_Y, m_chunk);
	}
}

BlockLocator BlockLocator::GetUp() const
{
	if (IsBlockIndexOnTopEdge(m_blockIndex))
	{
		return BlockLocator(m_blockIndex, nullptr);
	} else {
		return BlockLocator(m_blockIndex + CHUNK_LAYER_DIMS_XY, m_chunk);
	}
}

BlockLocator BlockLocator::GetDown() const
{
	if (IsBlockIndexOnBottomEdge(m_blockIndex))
	{
		return BlockLocator(m_blockIndex, nullptr);
	} else {
		return BlockLocator(m_blockIndex - CHUNK_LAYER_DIMS_XY, m_chunk);
	}
}

void BlockLocator::MoveEast()
{
	if (IsBlockIndexOnEastEdge(m_blockIndex)){
		m_chunk = m_chunk->m_eastNeighbor;
		//x is now the westernmost block, which is 0
		m_blockIndex &= (~CHUNK_MASK_X);
	} else {
		m_blockIndex++;
	}
}

void BlockLocator::MoveWest()
{
	if (IsBlockIndexOnWestEdge(m_blockIndex)){
		m_chunk = m_chunk->m_westNeighbor;
		//x is now the eastern block, which is 15
		m_blockIndex |= (CHUNK_MASK_X);
	} else {
		m_blockIndex++;
	}
}

void BlockLocator::MoveSouth()
{
	if (IsBlockIndexOnSouthEdge(m_blockIndex)){
		m_chunk = m_chunk->m_southNeighbor;
		//y is now the northern block, which is 15
		m_blockIndex |= CHUNK_MASK_Y;		//want to just like shwoop out the x mask
	} else {
		m_blockIndex -= CHUNK_SIZE_Y;
	}
}

void BlockLocator::MoveNorth()
{
	if (IsBlockIndexOnNorthEdge(m_blockIndex)){
		m_chunk = m_chunk->m_northNeighbor;
		//y is now the southern block, which is 0
		m_blockIndex &= (~CHUNK_MASK_Y);		//want to just like shwoop out the x mask
	} else {
		m_blockIndex += CHUNK_SIZE_Y;
	}
}

void BlockLocator::MoveUp()
{
	if (IsBlockIndexOnTopEdge(m_blockIndex))
	{
		//it's invalid! We're off the map!
		m_chunk = nullptr;
	} else {
		m_blockIndex+= CHUNK_LAYER_DIMS_XY;
	}
}

void BlockLocator::MoveDown()
{
	if (IsBlockIndexOnBottomEdge(m_blockIndex))
	{
		//it's invalid! We're off the map!
		m_chunk = nullptr;
	} else {
		m_blockIndex-= CHUNK_LAYER_DIMS_XY;
	}
}
