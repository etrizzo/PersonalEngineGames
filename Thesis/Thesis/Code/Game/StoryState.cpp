#include "StoryState.hpp"

StoryState::StoryState(float cost)
{
	m_cost = cost;
}

float StoryState::GetCost() const
{
	return m_cost;
}
