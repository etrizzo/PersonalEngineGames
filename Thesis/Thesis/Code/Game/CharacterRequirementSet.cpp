#include "CharacterRequirementSet.hpp"
#pragma once

void CharacterRequirementSet::InitFromXML(tinyxml2::XMLElement * setElement)
{
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
