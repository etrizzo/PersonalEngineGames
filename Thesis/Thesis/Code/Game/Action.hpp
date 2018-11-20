#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ActionDefinition.hpp"

class Action{
public:
	Action(ActionDefinition* def);

	std::string m_instancedText;
	float m_instancedChance;
	ActionDefinition* m_definition;
};