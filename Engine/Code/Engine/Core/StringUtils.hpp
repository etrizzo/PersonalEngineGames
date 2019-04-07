#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdint.h>

//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::string Stringv( const char* format, va_list args);

typedef std::vector<std::string> Strings;

void Split(std::string stringToSplit, const char splitOn, Strings& splitVector);
void Strip(std::string& stringToStrip, const char stripChar);
bool Contains(std::string baseWord, std::string subString );
bool StartsWith(std::string baseWord, std::string startString);

std::string ToString(bool boolean);

bool ContainsNumber(std::string stringToCheck);

std::string ToLower(std::string stringIn);

void* FileReadToNewBuffer( char const *filename );

template<typename T>
inline std::string ToBitString(T data)
{
	size_t sizeOfData = sizeof(data);
	size_t maxTries = sizeOfData * 8;
	size_t tries = 0U;
	size_t check = 1U;
	std::string bitString = "";
	while ( tries < maxTries){
		if (data & check){
			bitString = "1" + bitString;
		} else {
			bitString = "0" + bitString;
		}
		data = data >> 1;
		tries++;
	}
	return bitString;
}
