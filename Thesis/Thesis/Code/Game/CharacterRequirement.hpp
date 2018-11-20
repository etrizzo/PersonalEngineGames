#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tags.hpp"


class Character;
class CharacterState;
class StoryDataDefinition;

class CharacterRequirement{
public:
	CharacterRequirement() {};
	CharacterRequirement(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element );

	virtual CharacterRequirement* Clone() const = 0;
	
	virtual bool PassesRequirement(CharacterState* character) = 0;

	unsigned int m_characterID = (unsigned int) -1;
	StoryDataDefinition* m_parentData;
};


class CharacterRequirement_Tag : public CharacterRequirement{
public:
	CharacterRequirement_Tag(){};
	CharacterRequirement_Tag(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element);

	CharacterRequirement* Clone() const;
	
	bool PassesRequirement(CharacterState* character) override;
	
	TagPair m_tag;

};

class CharacterRequirement_Trait : public CharacterRequirement{
public:
	CharacterRequirement_Trait(){};
	CharacterRequirement_Trait(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element);

	CharacterRequirement* Clone() const;

	bool PassesRequirement(CharacterState* character) override;

	eCharacterTrait m_trait		= NUM_CHARACTER_TRAITS;
	FloatRange m_traitRange		= FloatRange(0.f);
	float m_wiggleroom			= 0.f;
};