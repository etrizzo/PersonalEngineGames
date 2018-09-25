#pragma once
#include "Game/GameCommon.hpp"

class StoryData{
public:
	StoryData(){};
	StoryData(std::string name, float value);
	StoryData(tinyxml2::XMLElement* nodeElement);
	~StoryData();

	std::string GetName() const;
	std::string ToString() const;
	
	void SetPosition(Vector2 pos);
	Vector2 GetPosition() const;

	bool operator==( const StoryData& compare ) const;
	bool operator!=( const StoryData& compare ) const;

	//test bits
	std::string m_name;
	float m_value;

	//actual members
	std::string m_id;
	std::string m_action;


	Vector2 m_graphPosition = Vector2::HALF;
};