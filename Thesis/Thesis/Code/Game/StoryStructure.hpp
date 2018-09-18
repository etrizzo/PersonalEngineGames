#pragma once
#include "Game/GameCommon.hpp"

// structure for keeping track of story state 
struct StoryStructure{
	unsigned int m_storyLength;
	std::vector<float> m_actionValues;
	std::vector<float> m_tensionValues;

	float GetActionAtTime (unsigned int time) const;
	float GetTensionAtTime(unsigned int time) const;

	void SetActionAtTime (unsigned int time, float value);
	void SetTensionAtTime(unsigned int time, float value);

};

