#pragma once
#include "Game/GameCommon.hpp"
#include "Game/CharacterRequirement.hpp"
#include "Game/CharacterState.hpp"

class CharacterRequirementSet{
public:
	CharacterRequirementSet(){};

	void InitFromXML(tinyxml2::XMLElement* setElement, StoryDataDefinition* parent);

	float GetCharacterFitness(Character * character, StoryState* edgeState);
	bool DoesCharacterMeetAllRequirements(Character * character, StoryState* edgeState);

	CharacterRequirementSet* Clone();
	void SetAllRequirementsStoryDataDefinition(StoryDataDefinition* parentData);
	
	unsigned int m_characterIndex;
	std::vector<CharacterRequirement*> m_requirements;	//TraitRequirements and TagRequirements both inherit from the same shit
};