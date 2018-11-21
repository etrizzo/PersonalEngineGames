#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tags.hpp"


class Character;
class CharacterState;
class StoryDataDefinition;

class StoryRequirement{
public:
	StoryRequirement() {};
	StoryRequirement(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element );

	virtual StoryRequirement* Clone() const = 0;

	virtual bool PassesRequirement(StoryState* edge) = 0;

	unsigned int m_characterID = (unsigned int) -1;
	StoryDataDefinition* m_parentData;
	float m_fitnessWeight = 1.f;
};


class StoryRequirement_Tag : public StoryRequirement{
public:
	StoryRequirement_Tag(){};
	StoryRequirement_Tag(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement* element);

	StoryRequirement* Clone() const;

	bool PassesRequirement(StoryState* edge) override;

	TagPair m_tag;

};