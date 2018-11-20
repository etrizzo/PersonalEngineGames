#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Effect.hpp"

class StoryDataDefinition;

class EffectSet{
public:
	EffectSet(){};
	EffectSet(tinyxml2::XMLElement* effectsElement, StoryDataDefinition* parent);
	EffectSet(EffectSet* clone, StoryDataDefinition* newParent);

	std::vector<Effect*> m_effects = std::vector<Effect*>();

	StoryDataDefinition* m_parentData = nullptr;
};