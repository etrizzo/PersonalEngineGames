#pragma once
#include "Engine/Math/MathUtils.hpp"

typedef unsigned char uchar;
constexpr uchar BLOCK_AIR = 0;

class BlockDefinition;

constexpr uchar BLOCK_BIT_IS_FULL_OPAQUE	= 0b0100'0000;
constexpr uchar BLOCK_BIT_IS_SKY			= 0b1000'0000;

constexpr uchar BLOCK_LIGHTING_BITS = 4;
constexpr uchar BLOCK_LIGHTING_INDOOR_MASK = (1 << BLOCK_LIGHTING_BITS) - 1;
constexpr uchar BLOCK_LIGHTING_OUTDOOR_MASK = BLOCK_LIGHTING_INDOOR_MASK << BLOCK_LIGHTING_BITS;


class Block
{
public:
	Block();
	Block( uchar blockType);

	inline uchar GetBlockID() const { return m_blockID; };

	void SetType(uchar newType);
	BlockDefinition* GetType() const;
	inline bool IsFullyOpaque() const;

	inline int GetIndoorLightLevel() const;
	inline int GetOutdoorLightLevel() const;

	uchar GetDefinitionInternalLightLevel() const;

private:
	unsigned char m_blockID;
	unsigned char m_bits;
	unsigned char m_lighting;		//outdoor lighting | indoor lighting
};


bool Block::IsFullyOpaque() const
{
	return AreBitsSet(m_bits, BLOCK_BIT_IS_FULL_OPAQUE);
}

inline int Block::GetIndoorLightLevel() const
{
	return m_bits & BLOCK_LIGHTING_INDOOR_MASK;
}

inline int Block::GetOutdoorLightLevel() const
{
	return (m_bits & BLOCK_LIGHTING_OUTDOOR_MASK) >> BLOCK_LIGHTING_BITS;
}
