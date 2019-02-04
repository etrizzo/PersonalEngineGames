#pragma once
#include "Game/GameCommon.hpp"
#include "Game/StoryRequirement.hpp"
#include "Game/StoryState.hpp"

//requirements for the overall story state. lives on StoryDataDefinition
class StoryRequirementSet{
public:
	StoryRequirementSet(){};

	void InitFromXML(tinyxml2::XMLElement* setElement, StoryDataDefinition* parent);

	float GetEdgeFitness(StoryState* edgeState);
	bool DoesEdgeMeetAllRequirements(StoryState* edgeState);

	void SetAllRequirementsStoryDataDefinition(StoryDataDefinition* parentData);

	unsigned int m_characterIndex;
	std::vector<StoryRequirement*> m_requirements;	//TraitRequirements and TagRequirements both inherit from the same shit
};