#pragma once
#include "Game/GameCommon.hpp"

class CharacterRequirementSet;
class StoryRequirementSet;
class ActionDefinition;
class StoryDataDefinition;

class ActionModifier{
public:
	ActionModifier(tinyxml2::XMLElement* actionElement, StoryDataDefinition* parent);
	int m_characterID;
	CharacterRequirementSet* m_characterRequirements;
	StoryRequirementSet* m_storyRequirements;
	ActionDefinition* m_parentDefinition;
	float m_multiplier;

	StoryDataDefinition* m_parentData;

	bool PassesForEdge(StoryData* parentData, StoryState* edge);

};