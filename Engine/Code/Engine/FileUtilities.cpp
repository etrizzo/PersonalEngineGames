#include "Engine/FileUtilities.hpp"

bool LoadBinaryFileIntoBuffer(std::string filePath, std::vector<unsigned char>& buffer)
{
	FILE *fp = nullptr;
	fopen_s( &fp, filePath.c_str(), "rb" );

	if (fp == nullptr)
	{
		//no file found.
		return false;
	}
	int c = 0;
	while (c != EOF)
	{
		c = fgetc(fp);
		buffer.push_back((unsigned char) c);
	}
	fclose(fp);
	return true;
}

