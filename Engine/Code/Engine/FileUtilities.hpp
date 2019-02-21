#pragma once
#include <string>
#include <vector>

#define MAX_LINE_LENGTH_FILE (99999999)

bool LoadBinaryFileIntoBuffer(std::string filePath, std::vector<unsigned char>& buffer);