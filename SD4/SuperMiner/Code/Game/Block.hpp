#pragma once

enum eBlockType : unsigned char{
	BLOCK_AIR = 0,			//air is always 0!! we make assumptions on this.
	BLOCK_GRASS,
	BLOCK_STONE,
	BLOCK_SNOW,
	BLOCK_SAND,
	BLOCK_COBBLESTONE,
	BLOCK_OAK_PLANKS,
	
	BLOCK_DIAMOND,
	BLOCK_GLASS,

	NUM_BLOCKS
};


class BlockDefinition;

class Block
{
public:
	Block();
	Block( eBlockType blockType);

	void SetType(eBlockType newType);
	BlockDefinition* GetType() const;
	bool IsFullyOpaque() const;

	unsigned char m_blockID;
	unsigned char m_bits;
};