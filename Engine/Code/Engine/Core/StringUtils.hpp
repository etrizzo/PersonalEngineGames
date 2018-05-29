#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

typedef std::vector<std::string> Strings;

const void Split(std::string stringToSplit, const char splitOn, Strings& splitVector);
const void Strip(std::string& stringToStrip, const char stripChar);
const bool Contains(std::string baseWord, std::string subString );
const bool StartsWith(std::string baseWord, std::string startString);

std::string ToLower(std::string stringIn);

void* FileReadToNewBuffer( char const *filename );