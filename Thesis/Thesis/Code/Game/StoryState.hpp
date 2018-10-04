#pragma once


class StoryState{
public:
	StoryState(float cost);
	StoryState(){};

	float m_cost;

	float GetCost() const;
};