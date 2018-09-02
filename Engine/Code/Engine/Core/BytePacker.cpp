#include "BytePacker.hpp"
#include "Engine/Core/EngineCommon.hpp"

BytePacker::BytePacker(eEndianness byte_order)
{
	m_byteOrder = byte_order;
	m_options = BYTEPACKER_CAN_GROW | BYTEPACKER_OWNS_MEMORY;
	m_maxSize = 8 * KB;
	m_buffer = malloc(8 * KB);
}

BytePacker::BytePacker(size_t buffer_size, eEndianness byte_order)
{
	m_byteOrder = byte_order;
	m_options = BYTEPACKER_OWNS_MEMORY;
	m_maxSize = buffer_size;
	m_buffer = malloc(buffer_size);
}

BytePacker::BytePacker(size_t buffer_size, void * buffer, eEndianness byte_order)
{
	m_byteOrder = byte_order;
	m_options = 0U;
	m_maxSize = buffer_size;
}

BytePacker::~BytePacker()
{
}

void BytePacker::SetEndianness(eEndianness newEndianness)
{
	if (m_byteOrder != newEndianness){
		m_byteOrder = newEndianness;
		ToEndianness(m_maxSize, m_buffer, m_byteOrder);
	}
	
}

bool BytePacker::SetReadableByteCount(size_t byte_count)
{
	if (AreBitsSet(m_options, BYTEPACKER_CAN_GROW) ){
		return false;
	} else {
		m_maxSize = byte_count;
	}
}

bool BytePacker::WriteBytes(size_t byte_count, void const * data, bool convertEndianness)
{
	//if byte count + current count > max size & you can't grow, return false
	if (byte_count + m_writeHead >= m_maxSize){
		if (!AreBitsSet(m_options, BYTEPACKER_CAN_GROW)){
			return false;
		} else {	// if you can grow, expand the buffer
			size_t newSize = m_maxSize * 4;
			void* newBuffer = malloc(newSize);
			memcpy(newBuffer, m_buffer, m_maxSize);
			free (m_buffer);

			m_buffer = newBuffer;
			
			m_maxSize = newSize;
		}
	}

	//, add data to buffer and increment currentByteSize
	
	// First: convert data to the bytepacker's endianness
	void* convertedData = malloc(byte_count);
	memcpy((byte_t*) convertedData, data, byte_count);
	if (convertEndianness){
		ToEndianness(byte_count, convertedData, GetEndianness());
	}

	// copy the converted data onto the buffer
	memcpy(((byte_t*) m_buffer) + m_writeHead, convertedData, byte_count);

	//free converted data
	free (convertedData);
	m_writeHead += byte_count;

	//return true
	return true;
}

size_t BytePacker::ReadBytes(void * out_data, size_t max_byte_count)
{
	//tries to read to out_data up to max_byte_count
	// returns number of bytes read
	size_t readable = GetReadableByteCount();
	size_t bytesRead = max_byte_count;
	if ( max_byte_count > readable){
		bytesRead = readable;
	} 
	memcpy(out_data, ((byte_t*) m_buffer) + m_readHead, bytesRead);
	m_readHead += bytesRead;
	FromEndianness(bytesRead, out_data, GetEndianness());
	return bytesRead;
}

size_t BytePacker::WriteSize(size_t size)
{
	// converts input size to the min number of bytes w/ weird bitmath

	size_t currentValue = size;
	size_t numBytesWritten = 0;
	
	while (currentValue != 0)
	{
		//	CONVERSION:
		//	1. AND size with  0b0111'1111 to get least significant 7 bits
		byte_t leastSignificantBits = currentValue & 0b0111'1111;

		//	2. if (size >> 7) == 0, done reading
		//	3. otherwise, OR leastSignificantBits with 0b1000'0000 to set signifier bits
		if ((currentValue >> 7) != 0){
			leastSignificantBits |= 0b1000'0000;
		}
		
		//	4. write byte and increment numBytesWritten
		WriteBytes(1, (void*) &(leastSignificantBits), false);
		numBytesWritten++;
		currentValue >>= 7;
	}

	return numBytesWritten;
}

size_t BytePacker::ReadSize(size_t * out_size)
{
	// de-compress a size and fill it into out_size
	bool keepReading = true;
	size_t bytesRead = 0;
	size_t sizeCounted = 0;
	while (keepReading){
		//	Conversion:
		//	0.5 (read a byte from b0offer)
		byte_t readByte;
		ReadBytes((void*) &readByte, 1);
		//	1. AND with 0b1000'0000 to see if this is the final bit
		keepReading = ((readByte & 0b1000'0000) != 0);
		//	2. AND byte with 0b0111'1111 to get least significant 7 bits
		size_t leastSignificantBits = readByte & 0b0111'1111;
		//	3. left-shift byte by (7 * bytesRead)
		leastSignificantBits <<= (7 * bytesRead);
		//	4. add result to out_size
		sizeCounted += leastSignificantBits;
		//	5. increment bytesRead;
		bytesRead++;
		//	6. Repeat
	}
	// return number of bytes read
	out_size = &sizeCounted;
	return bytesRead;
}

bool BytePacker::WriteString(char const * str)
{
	// converts str to std::string to get size
	std::string asString = std::string(str);
	size_t sizeOfString = asString.size();
	// write the size of the string
	WriteSize(sizeOfString);
	// toEndianness(str, GetEndianness())
	// calls writebytes(str)
	bool wrote = true;
	for (size_t i = 0; i < sizeOfString; i++){
		// write each byte individually for reasons???????? Endianness???????
		wrote = WriteBytes(1, (void*) &str[i]);
	}
	return wrote;
}

size_t BytePacker::ReadString(char * out_str, size_t max_byte_size)
{
	// calls readsize()
	size_t sizeToRead = 0;
	ReadSize(&sizeToRead);
	sizeToRead = Min((int) sizeToRead, (int) max_byte_size);
	ReadBytes((void*) out_str, sizeToRead);
	// calls readbytes(size0;
	return sizeToRead;
}

void BytePacker::ResetWrite()
{
	// re-set write_head to start of buffer
	// DUMP MEMORY 
	m_writeHead = 0;
	free (m_buffer);
}

void BytePacker::ResetRead()
{
	//re-set read_head
	m_readHead = 0;
}

eEndianness BytePacker::GetEndianness() const
{
	return m_byteOrder;
}

size_t BytePacker::GetWrittenByteCount() const
{
	return m_writeHead;
}

size_t BytePacker::GetWritableByteCount() const
{
	return m_maxSize - m_writeHead;
}

size_t BytePacker::GetReadableByteCount() const
{
	return m_writeHead - m_readHead;
}