#include "CharacterRequirementSet.hpp"
#pragma once

void CharacterRequirementSet::InitFromXML(tinyxml2::XMLElement * setElement, StoryDataDefinition* parent)
{
	m_characterIndex = (unsigned int) ParseXmlAttribute(*setElement, "index", (int) -1);

	for (tinyxml2::XMLElement* reqElement = setElement->FirstChildElement("Requirement"); reqElement != nullptr; reqElement = reqElement->NextSiblingElement("Requirement")){
		//create new character requirement
		CharacterRequirement* newRequirement = nullptr;
		std::string traitType = ParseXmlAttribute(*reqElement, "trait", "NO_TRAIT");
		std::string tagName = ParseXmlAttribute(*reqElement, "tag", "NO_TAG");
		if (traitType != "NO_TRAIT"){
			newRequirement = (CharacterRequirement*) new CharacterRequirement_Trait(m_characterIndex, parent, reqElement);
		} else if (tagName != "NO_TAG"){
			newRequirement = (CharacterRequirement*) new CharacterRequirement_Tag(m_characterIndex, parent, reqElement);
		}
		if (newRequirement != nullptr){
			m_requirements.push_back(newRequirement);
		} else {
			ConsolePrintf(RGBA::RED, "Error parsing character requirements - no identified requirement type");
		}
	}
}

float CharacterRequirementSet::GetCharacterFitness(CharacterState * character)
{
	float baseFitness = 0.f;
	for (CharacterRequirement* req : m_requirements){
		if (req->PassesRequirement(character)){
			baseFitness += req->m_fitnessWeight;
		} else {
			baseFitness -= req->m_fitnessWeight;
		}
	}
	return baseFitness;
}

bool CharacterRequirementSet::DoesCharacterMeetAllRequirements(CharacterState* character)
{
	for (CharacterRequirement* req : m_requirements){
		if (!req->PassesRequirement(character)){
			return false;
		}
	}
	return true;
}

CharacterRequirementSet * CharacterRequirementSet::Clone()
{
	CharacterRequirementSet* newSet = new CharacterRequirementSet();
	newSet->m_characterIndex = m_characterIndex;
	newSet->m_requirements = std::vector<CharacterRequirement*>();
	for (CharacterRequirement* localReq : m_requirements){
		newSet->m_requirements.push_back(localReq->Clone());
	}
	return newSet;
}

void CharacterRequirementSet::SetAllRequirementsStoryDataDefinition(StoryDataDefinition * parentData)
{
	for (CharacterRequirement* req : m_requirements){
		req->m_parentData = parentData;
	}
}
