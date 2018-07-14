#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

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

std::string ToLower(std::string stringIn);

void* FileReadToNewBuffer( char const *filename );