#pragma once
// Core/Endianness.hpp or similar
enum eEndianness 
{
	LITTLE_ENDIAN, 
	BIG_ENDIAN, 
};

eEndianness GetPlatformEndianness(); 

// Assumes data is in platform endianness, and will convert to supplied endianness; 
void ToEndianness( size_t const size, void *data, eEndianness desiredEndianness );

// Assumes data is in supplied endianness, and will convert to platform's endianness
void FromEndianness( size_t const size, void *data, eEndianness sourceEndianness ); 


