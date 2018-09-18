#pragma once
#include "Game/GameCommon.hpp"

class StoryData{
public:
	StoryData(){};
	StoryData(std::string name, float value);
	~StoryData();

	std::string GetName() const;
	std::string ToString() const;
	
	void SetPosition(Vector2 pos);
	Vector2 GetPosition() const;

	bool operator==( const StoryData& compare ) const;
	bool operator!=( const StoryData& compare ) const;

	//test
	std::string m_name;
	float m_value;

	Vector2 m_graphPosition = Vector2::HALF;
};