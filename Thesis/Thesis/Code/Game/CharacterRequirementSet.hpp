#pragma once
#include "Game/GameCommon.hpp"
#include "Game/CharacterRequirement.hpp"
#include "Game/CharacterState.hpp"

class CharacterRequirementSet{
public:
	CharacterRequirementSet(){};

	void InitFromXML(tinyxml2::XMLElement* setElement);

	bool DoesCharacterMeetRequirements(CharacterState* character);
	
	unsigned int m_characterIndex;
	std::vector<CharacterRequirement*> m_requirements;	//TraitRequirements and TagRequirements both inherit from the same shit
};