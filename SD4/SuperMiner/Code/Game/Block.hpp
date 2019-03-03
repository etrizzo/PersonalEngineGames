#pragma once
#include "Engine/Math/MathUtils.hpp"

typedef unsigned char uchar;
constexpr uchar BLOCK_AIR = 0;

class BlockDefinition;

constexpr uchar BLOCK_BIT_IS_SKY			= 0b1000'0000;
constexpr uchar BLOCK_BIT_IS_FULL_OPAQUE	= 0b0100'0000;
constexpr uchar BLOCK_BIT_IS_LIGHT_DIRTY	= 0b0010'0000;
constexpr uchar BLOCK_BIT_IS_SOLID			= 0b0001'0000;
constexpr uchar BLOCK_BIT_IS_VISIBLE		= 0b0000'1000;

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
	inline bool IsSky() const;
	inline bool IsLightDirty() const;
	inline bool IsSolid() const;
	inline bool IsVisible() const;

	inline void SetLightDirty();
	inline void ClearLightDirty();

	inline void SetIndoorLighting(uchar indoor);
	inline void SetOutdoorLighting(uchar outdoor);

	inline int GetIndoorLightLevel() const;
	inline int GetOutdoorLightLevel() const;


	uchar GetDefinitionInternalLightLevel() const;

private:
	unsigned char m_blockID		= 0U;
	unsigned char m_bits		= 0U;
	unsigned char m_lighting	= 0U;		//outdoor lighting | indoor lighting
};


bool Block::IsFullyOpaque() const
{
	return AreBitsSet(m_bits, BLOCK_BIT_IS_FULL_OPAQUE);
}

inline bool Block::IsSky() const
{
	return AreBitsSet(m_bits, BLOCK_BIT_IS_SKY);
}

inline bool Block::IsLightDirty() const
{
	return AreBitsSet(m_bits, BLOCK_BIT_IS_LIGHT_DIRTY);
}

inline bool Block::IsSolid() const
{
	return AreBitsSet(m_bits, BLOCK_BIT_IS_SOLID);
}

inline bool Block::IsVisible() const
{
	return AreBitsSet(m_bits, BLOCK_BIT_IS_VISIBLE);
}

inline void Block::SetLightDirty()
{
	SetBits(m_bits, BLOCK_BIT_IS_LIGHT_DIRTY);
}

inline void Block::ClearLightDirty()
{
	ClearBits(m_bits, BLOCK_BIT_IS_LIGHT_DIRTY);
}

inline void Block::SetIndoorLighting(uchar indoor)
{
	ClearBits(m_lighting, BLOCK_LIGHTING_INDOOR_MASK);
	m_lighting |= indoor;
}

inline void Block::SetOutdoorLighting(uchar outdoor)
{
	ClearBits(m_lighting, BLOCK_LIGHTING_OUTDOOR_MASK);
	m_lighting |= (outdoor << BLOCK_LIGHTING_BITS);
}

inline int Block::GetIndoorLightLevel() const
{
	return m_lighting & BLOCK_LIGHTING_INDOOR_MASK;
}

inline int Block::GetOutdoorLightLevel() const
{
	return (m_lighting  & BLOCK_LIGHTING_OUTDOOR_MASK) >> BLOCK_LIGHTING_BITS;
}
