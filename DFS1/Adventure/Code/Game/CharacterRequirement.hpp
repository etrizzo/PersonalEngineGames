#pragma once
#include "Game/GameCommon.hpp"
#include "Game/TagSet.hpp"


class Character;
class CharacterState;
class StoryDataDefinition;
class StoryState;

class CharacterRequirement{
public:
	CharacterRequirement() {};
	CharacterRequirement(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element );

	virtual CharacterRequirement* Clone() const = 0;
	
	virtual bool PassesRequirement(Character* character, StoryState* edgeState) = 0;

	unsigned int m_characterID = (unsigned int) -1;
	StoryDataDefinition* m_parentData;
	float m_fitnessWeight = 1.f;
};


//a tag on the character state
class CharacterRequirement_Tag : public CharacterRequirement{
public:
	CharacterRequirement_Tag(){};
	CharacterRequirement_Tag(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element);

	CharacterRequirement* Clone() const;
	
	bool PassesRequirement(Character* character, StoryState* edgeState) override;
	
	TagPair m_tag;

};

//character matches a tag on the story state
class CharacterRequirement_StoryTag : public CharacterRequirement{
public:
	CharacterRequirement_StoryTag(){};
	CharacterRequirement_StoryTag(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element);

	CharacterRequirement* Clone() const;

	bool PassesRequirement(Character* character, StoryState* edgeState) override;

	std::string m_tagName = "NONE";
};
