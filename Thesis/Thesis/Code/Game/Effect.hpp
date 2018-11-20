#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tags.hpp"

class StoryState;
class StoryData;
class StoryDataDefinition;
class CharacterState;

class Effect{
public:
	Effect(){};
	Effect( tinyxml2::XMLElement* element, StoryDataDefinition* parentData);

	virtual bool ApplyToState(StoryState* state, StoryData* instancedData) = 0;
	virtual bool ApplyToCharacterState(CharacterState* state) = 0;
	virtual Effect* Clone() = 0;

	unsigned int m_characterID = (unsigned int) -1;
	StoryDataDefinition* m_parentData = nullptr;
};


class Effect_TagChange : public Effect{
public:
	Effect_TagChange(){};
	Effect_TagChange(tinyxml2::XMLElement* element, StoryDataDefinition* parentData);

	bool ApplyToState(StoryState* state, StoryData* instancedData) override;
	bool ApplyToCharacterState(CharacterState* state) override;
	Effect* Clone() override;

	TagPair m_tag;

};

class Effect_TraitChange : public Effect{
public:
	Effect_TraitChange(){};
	Effect_TraitChange(tinyxml2::XMLElement* element, StoryDataDefinition* parentData);


	bool ApplyToState(StoryState* state, StoryData* instancedData) override;
	bool ApplyToCharacterState(CharacterState* state) override;
	Effect* Clone() override;

	eCharacterTrait m_trait		= NUM_CHARACTER_TRAITS;
	FloatRange m_traitRange		= FloatRange(0.f);
};