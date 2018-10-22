#pragma once
#include "Game/Tags.hpp"
#include "Game/Character.hpp"


class CharacterState{
public:
	CharacterState(Character* parent);
	CharacterState(CharacterState* previousState);

	std::string ToString() const;

	bool IsTraitInRange(eCharacterTrait trait, const FloatRange& desiredRange, float wiggleroom = 0.f);
	bool IsTraitInRange(eCharacterTrait trait, float min, float max, float wiggleroom = 0.f);

	bool HasTag(TagPair tag);

	Character* m_character;
	Tags m_tags;

	//Note: Check with squirrel on how to do this...
	FloatRange m_traits[NUM_CHARACTER_TRAITS] = { 
		FloatRange(0.f,100.f),
		FloatRange(0.f,100.f), 
		FloatRange(0.f,100.f), 
		FloatRange(0.f,100.f)
	};
};