#pragma once
#include "Game/GameCommon.hpp"

class Block
{
public:
	Block();
	Block( eBlockType blockType);

	void SetType(eBlockType newType);

	unsigned char m_blockID;
	unsigned char m_bits;
};