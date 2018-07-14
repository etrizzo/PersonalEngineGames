#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>





//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

const std::string Stringv(const char * format, va_list args)
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, args );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );

}

 void Split(std::string stringToSplit, const char splitOn, Strings & splitVector)
{
	splitVector = std::vector<std::string>();
	unsigned int oldIndex = 0;
	size_t newIndex = (size_t) stringToSplit.find(splitOn);
	while (newIndex != std::string::npos){
		std::string indexString = stringToSplit.substr(oldIndex,newIndex - oldIndex);
		if (indexString != ""){
			splitVector.push_back(indexString); 
		}

		oldIndex = newIndex + 1;
		newIndex = (size_t) stringToSplit.find(splitOn, oldIndex);
	}
	std::string indexString = stringToSplit.substr(oldIndex);
	if (indexString != ""){
		splitVector.push_back(indexString);
	}
}

//strips any instances of stripChar off the ends of stringToStrip
 void Strip(std::string & stringToStrip, const char stripChar)
{
	int endPos = (int) stringToStrip.find_last_not_of(stripChar);
	int startPos = (int) stringToStrip.find_first_not_of(stripChar);

	if (startPos >= 0 && endPos >= 0){
		stringToStrip = stringToStrip.substr(startPos, endPos + 1);
	}
}

 bool Contains(std::string baseWord, std::string subString)
{
	//TODO("This is not perfect :(");
	if (subString == ""){
		return true;
	}
	bool checking = false;
	int subIdx = 0;
	for (int baseIdx = 0; baseIdx < (int) baseWord.size(); baseIdx++){
		if (checking){
			if (subIdx >= (int) subString.size()){
				return true;		//you found the whole substring!!
			}
			if (baseWord[baseIdx] != subString[subIdx]){
				checking =false;
				//break the current check
			} else {
				subIdx++;
			}
		} else{
			if (baseWord[baseIdx] == subString[0]){		//start a check
				checking = true;
				subIdx = 1;
			}
		}
	}
	if (checking && subIdx == (int)subString.size()){
		return true;
	}
	return false;
}

 bool StartsWith(std::string baseWord, std::string startString)
{
	//all words start with "" :)
	if (startString == ""){
		return true;
	}
	//if start string longer than baseword, baseword can't begin with startstring
	if (startString.size() > baseWord.size()){
		return false;
	}


	for (int i = 0; i < (int) startString.size(); i++){
		//if start of baseword doesn't match exactly, gtfo
		if (baseWord[i] != startString[i]){
			return false;
		}
	}
	//made it through entire startString
	return true;
}

std::string ToLower(std::string stringIn)
{
	std::string out = stringIn;
	for (int i = 0 ; i < (int) stringIn.length(); i++){
		out[i] = (char) tolower(stringIn[i]);
	}
	return out;
}


void* FileReadToNewBuffer( char const *filename )
{
	FILE *fp = nullptr;
	fopen_s( &fp, filename, "r" );

	if (fp == nullptr) {
		return nullptr;
	}

	size_t size = 0U;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	unsigned char* buffer = (unsigned char*) malloc(size + 1U); // space for NULL

	size_t read = fread( buffer, 1, size, fp );
	fclose(fp);

	buffer[read] = NULL;

	return buffer; 
}


