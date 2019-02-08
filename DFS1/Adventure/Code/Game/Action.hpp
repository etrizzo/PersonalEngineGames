#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ActionDefinition.hpp"

class StoryData;
class StoryState;

class Action{
public:
	Action(ActionDefinition* def);

	std::string m_instancedText;
	float m_instancedChance;
	ActionDefinition* m_definition;

	bool DoesActionEndAct() const;
	void SetChanceFromEdge(StoryData* parentData, StoryState* edge);
};