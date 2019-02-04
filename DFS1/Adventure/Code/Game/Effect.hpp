#pragma once
#include "Game/GameCommon.hpp"
#include "Game/TagSet.hpp"

class StoryState;
class StoryData;
class StoryDataDefinition;
class CharacterState;

enum eEffectType{
	EFFECT_TYPE_CHARACTER,
	EFFECT_TYPE_STORY,
	NUM_EFFECT_TYPES
};

class Effect{
public:
	Effect(){};
	Effect( tinyxml2::XMLElement* element, StoryDataDefinition* parentData);

	virtual bool ApplyToState(StoryState* state, StoryData* instancedData) = 0;
	virtual bool ApplyToCharacterState(CharacterState* state) = 0;
	virtual Effect* Clone() = 0;

	eEffectType m_type = EFFECT_TYPE_CHARACTER;
	std::string m_characterIndexString;
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

