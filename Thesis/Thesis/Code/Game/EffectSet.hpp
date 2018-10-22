#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Effect.hpp"

class StoryData;

class EffectSet{
public:
	EffectSet(){};
	EffectSet(tinyxml2::XMLElement* effectsElement, StoryData* parent);
	EffectSet(EffectSet* clone, StoryData* newParent);

	std::vector<Effect*> m_effects;

	StoryData* m_parentData;
};