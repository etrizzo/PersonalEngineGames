#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tags.hpp"

class Character;
class CharacterState;

class CharacterRequirement{
public:
	CharacterRequirement(unsigned int charID, tinyxml2::XMLElement* element );
	
	virtual bool PassesRequirement(CharacterState* character) = 0;

	unsigned int m_characterID = (unsigned int) -1;
};


class CharacterRequirement_Tag : public CharacterRequirement{
public:
	CharacterRequirement_Tag(unsigned int charID, tinyxml2::XMLElement* element);
	
	bool PassesRequirement(CharacterState* character) override;
	
	TagPair m_tag;

};

class CharacterRequirement_Trait : public CharacterRequirement{
public:
	CharacterRequirement_Trait(unsigned int charID, tinyxml2::XMLElement* element);

	bool PassesRequirement(CharacterState* character) override;

	eCharacterTrait m_trait		= NUM_CHARACTER_TRAITS;
	FloatRange m_traitRange		= FloatRange(0.f);
	float m_wiggleroom			= 0.f;
};