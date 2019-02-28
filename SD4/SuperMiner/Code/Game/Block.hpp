#pragma once

typedef unsigned char uchar;
constexpr unsigned char BLOCK_AIR = 0;

//enum uchar : unsigned char{
//	BLOCK_AIR = 0,			//air is always 0!! we make assumptions on this.
//	BLOCK_GRASS,
//	BLOCK_STONE,
//	BLOCK_SNOW,
//	BLOCK_SAND,
//	BLOCK_COBBLESTONE,
//	BLOCK_OAK_PLANKS,
//	BLOCK_DIAMOND,
//	BLOCK_GLASS,
//	BLOCK_GLOWSTONE,
//
//	NUM_BLOCKS
//};


class BlockDefinition;

constexpr unsigned char BLOCK_BIT_IS_FULL_OPAQUE	= 0b0100'0000;
constexpr unsigned char BLOCK_BIT_IS_SKY			= 0b1000'0000;

class Block
{
public:
	Block();
	Block( uchar blockType);

	void SetType(uchar newType);
	BlockDefinition* GetType() const;
	bool IsFullyOpaque() const;

	unsigned char m_blockID;
	unsigned char m_bits;
	unsigned char m_lighting;		//outdoor lighting | indoor lighting
};