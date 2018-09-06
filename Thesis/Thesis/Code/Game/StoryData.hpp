#pragma once
#include "Game/GameCommon.hpp"

class StoryData{
public:
	StoryData(){};
	StoryData(std::string name, float value);
	~StoryData();

	std::string GetName() const;
	std::string ToString() const;

	bool operator==( const StoryData& compare ) const;
	bool operator!=( const StoryData& compare ) const;

	std::string m_name;
	float m_value;
};