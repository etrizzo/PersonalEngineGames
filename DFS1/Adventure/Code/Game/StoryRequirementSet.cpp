#include "StoryRequirementSet.hpp"
#pragma once

void StoryRequirementSet::InitFromXML(tinyxml2::XMLElement * setElement, StoryDataDefinition* parent)
{
	if (setElement != nullptr){
		m_characterIndex = (unsigned int) ParseXmlAttribute(*setElement, "index", (int) -1);

		for (tinyxml2::XMLElement* reqElement = setElement->FirstChildElement("Requirement"); reqElement != nullptr; reqElement = reqElement->NextSiblingElement("Requirement")){
			//create new character requirement
			StoryRequirement* newRequirement = nullptr;
			std::string traitType = ParseXmlAttribute(*reqElement, "trait", "NO_TRAIT");
			std::string tagName = ParseXmlAttribute(*reqElement, "tag", "NO_TAG");
			if (traitType != "NO_TRAIT"){
				//newRequirement = (StoryRequirement*) new StoryRequirement_Trait(m_characterIndex, parent, reqElement);
			} else if (tagName != "NO_TAG"){
				newRequirement = (StoryRequirement*) new StoryRequirement_Tag(m_characterIndex, parent, reqElement);
			}
			if (newRequirement != nullptr){
				m_requirements.push_back(newRequirement);
			} else {
				ConsolePrintf(RGBA::RED, "Error parsing story requirements - no identified requirement type");
			}
		}
	}
}

float StoryRequirementSet::GetEdgeFitness(StoryState * character)
{
	float baseFitness = 0.f;
	for (StoryRequirement* req : m_requirements){
		if (req->PassesRequirement(character)){
			baseFitness += req->m_fitnessWeight;
		} else {
			baseFitness -= req->m_fitnessWeight;
		}
	}
	return baseFitness;
}

bool StoryRequirementSet::DoesEdgeMeetAllRequirements(StoryState* character)
{
	for (StoryRequirement* req : m_requirements){
		if (!req->PassesRequirement(character)){
			return false;
		}
	}
	return true;
}



void StoryRequirementSet::SetAllRequirementsStoryDataDefinition(StoryDataDefinition * parentData)
{
	for (StoryRequirement* req : m_requirements){
		req->m_parentData = parentData;
	}
}
