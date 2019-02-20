#pragma once
#include "Game/GameCommon.hpp"


class Chunk;
class BlockDefinition;

class BlockLocator
{
public:
	BlockLocator(int blockIndex, Chunk* chunk);
	Chunk* m_chunk;
	int m_blockIndex;

	bool operator==(const BlockLocator& other);
	bool operator!=(const BlockLocator& other);

	Block& GetBlock() const;
	BlockDefinition* GetBlockType() const;
	inline bool IsBlockFullyOpaque() const;			
	AABB3 GetBlockBounds() const;
	Vector3 GetBlockCenterWorldPosition() const;

	//Functions that get make block locators for your neighbors
	BlockLocator GetEast() const;
	BlockLocator GetWest() const;
	BlockLocator GetSouth() const;
	BlockLocator GetNorth() const;
	BlockLocator GetUp() const;
	BlockLocator GetDown() const;

	//Functions that move this block locator
	void MoveEast();
	void MoveWest();
	void MoveSouth();
	void MoveNorth();
	void MoveUp();
	void MoveDown();
};

bool BlockLocator::IsBlockFullyOpaque() const
{
	return GetBlock().IsFullyOpaque();
}

