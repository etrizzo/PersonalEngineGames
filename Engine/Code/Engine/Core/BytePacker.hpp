// Core/BytePacker.hpp or equivalent
#pragma once
#include "Engine/Core/Endianness.hpp"

 #define BIT_FLAG(f)   (1U << (f))

// Engine/Core/BytePacker.hpp
enum eBytePackerOptionBit : unsigned int
{
	// a byte packer allocated its own memory and 
	// should free it when destroyed; 
	BYTEPACKER_OWNS_MEMORY    = BIT_FLAG(0),

	// If the bytepacker runs out of space, will
	// allocate more memory (maintaining the data that was stored)
	// Must have flag BYTEPACKER_OWNS_MEMORY
	BYTEPACKER_CAN_GROW       = BIT_FLAG(1)
};
typedef unsigned int eBytePackerOptions; 


// Special notes 
// THIS IS NOT A COMPLETE LIST of methods you 
class BytePacker 
{
public:     
	//just saves byte order, can grow indefinitely in size and we assume you own memory
	BytePacker( eEndianness byte_order = LITTLE_ENDIAN ); 
	// pre-set a buffer size and byte order, can't grow, own memory
	BytePacker( size_t buffer_size, eEndianness byte_order = LITTLE_ENDIAN ); 
	// pre-set max size and the buffer to write to (with byte order ass well), can't grow, don't own
	BytePacker( size_t buffer_size, void *buffer, eEndianness byte_order = LITTLE_ENDIAN ); 
	~BytePacker(); 

	// sets how much of the buffer is readable; 
	void SetEndianness( eEndianness newEndianness ); 
	bool SetReadableByteCount( size_t byte_count );		//is this max size or readable data?
	void AdvanceWriteHead(size_t bytesToAdvance);

	// tries to write data to the end of the buffer;  Returns success
	bool WriteBytes( size_t byte_count, void const *data, bool convertEndianness = true); 
	// Tries to read into out_data.  Returns how much
	// ended up being read; 
	size_t ReadBytes( void *out_data, size_t max_byte_count );

	//compressing function
	size_t WriteSize( size_t size ); // returns how many bytes used
	size_t ReadSize( size_t *out_size ); // returns how many bytes read, fills out_size

	 // See notes on encoding!
	bool WriteString( char const *str ); 
	size_t ReadString( char *out_str, size_t max_byte_size ); // max_str_size should be enough to contain the null terminator as well; 

	void* GetWriteHeadLocation() const;
	void* GetBuffer() const;
															   // HELPERS
															   // suggested method names for commonly needed information; 
	void ResetWrite();  // resets writing to the beginning of the buffer.  Make sure read head stays valid (<= write_head)
	void ResetRead();   // resets reading to the beginning of the buffer

	eEndianness GetEndianness() const; 
	size_t GetWrittenByteCount() const;    // how much have I written to this buffer
	size_t GetWritableByteCount() const;   // how much more can I write to this buffer (if growble, this returns UINFINITY)
	size_t GetReadableByteCount() const;   // how much more data can I read;


private:
	// TODO: Figure out what members and methods
	// you will need to do the above; 

	eEndianness m_byteOrder;
	eBytePackerOptions m_options;
	size_t m_maxSize;
	size_t m_readHead  = 0U;
	size_t m_writeHead = 0U;	 //size of total readable data
	void* m_buffer;
}; 