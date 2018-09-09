#include "Endianness.hpp"
#include "Engine/Core/EngineCommon.hpp"

eEndianness GetPlatformEndianness()
{
	const unsigned int num = 1;		//just check if the first byte in a (byte_t*) cast is 1, it's little endian
	const byte_t* array = (const byte_t*)&num;
	if (array[0] == 0x01) {
		return LITTLE_ENDIAN;
	} else {
		return BIG_ENDIAN;
	}
}

void ToEndianness(size_t const size, void * data, eEndianness desiredEndianness)
{
	if (GetPlatformEndianness() == desiredEndianness)
	{
		return;
	}

	byte_t* byte_buffer = (byte_t*) data;

	//otherwise, you need to swap from little endian to big endian
	//basically, just reversing the bytes
	unsigned int i = 0;
	unsigned int j = (unsigned int) size - 1U;
	while (i < j){
		std::swap(byte_buffer[i], byte_buffer[j]);
		++i;
		--j;
	}
}

void FromEndianness(size_t const size, void * data, eEndianness sourceEndianness)
{
	if (GetPlatformEndianness() == sourceEndianness)
	{
		return;
	}

	byte_t* byte_buffer = (byte_t*) data;

	//otherwise, you need to swap from little endian to big endian
	//basically, just reversing the bytes
	unsigned int i = 0;
	unsigned int j = (unsigned int) size - 1U;
	while (i < j){
		std::swap(byte_buffer[i], byte_buffer[j]);
		++i;
		--j;
	}
}
