#include "CharacterRequirementSet.hpp"
#pragma once

void CharacterRequirementSet::InitFromXML(tinyxml2::XMLElement * setElement)
{
	m_characterIndex = (unsigned int) ParseXmlAttribute(*setElement, "index", (int) -1);

	for (tinyxml2::XMLElement* reqElement = setElement->FirstChildElement("Requirement"); reqElement != nullptr; reqElement = reqElement->NextSiblingElement("Requirement")){
		//create new character requirement
		CharacterRequirement* newRequirement = nullptr;
		std::string traitType = ParseXmlAttribute(*reqElement, "trait", "NO_TRAIT");
		std::string tagName = ParseXmlAttribute(*reqElement, "tag", "NO_TAG");
		if (traitType != "NO_TRAIT"){
			newRequirement = (CharacterRequirement*) new CharacterRequirement_Trait(m_characterIndex, reqElement);
		} else if (tagName != "NO_TAG"){
			newRequirement = (CharacterRequirement*) new CharacterRequirement_Tag(m_characterIndex, reqElement);
		}
		if (newRequirement != nullptr){
			m_requirements.push_back(newRequirement);
		} else {
			ConsolePrintf(RGBA::RED, "Error parsing character requirements - no identified requirement type");
		}
	}
}

bool CharacterRequirementSet::DoesCharacterMeetRequirements(CharacterState* character)
{
	for (CharacterRequirement* req : m_requirements){
		if (!req->PassesRequirement(character)){
			return false;
		}
	}
	return true;
}
