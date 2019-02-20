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

	Block& GetBlock() const;
	BlockDefinition* GetBlockType() const;
	bool IsBlockFullyOpaque() const			{ return GetBlock().IsFullyOpaque(); }

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
